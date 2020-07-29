#include "Settings.h"

#include "drivers/EERAM.h"

#include <coredecls.h> // crc32()

Settings::Settings()
{
    static_assert(
        sizeof(Data) <= Config::EeramAddresses::SchedulerDataBase - Config::EeramAddresses::SettingsBase,
        "Settings data doesn't fit in the allocated space"
    );

    _log.info("initializing");

    // Disable ASE by default to avoid unnecessary wearing when settings are not changed
    Drivers::EERAM::StatusReg sr;
    sr.value = 0;
    Drivers::EERAM::setStatus(sr);
}

void Settings::loadDefaults()
{
    _log.info("loading default settings");

    data = {};
}

void Settings::load()
{
    _log.info(
        "loading settings from EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        sizeof data
    );

    const auto ok = Drivers::EERAM::read(
        Config::EeramAddresses::SettingsBase,
        reinterpret_cast<uint8_t*>(&data),
        sizeof data
    );

    if (!ok) {
        _log.error("failed to read settings from EERAM");
    }

    if (calculateDataChecksum() != data.checksum)
    {
        _log.warning("checksum error");

        loadDefaults();
        save();
    }

    // TODO print settings
}

void Settings::save()
{
    _log.debug(
        "saving settings to EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        sizeof data
    );

    data.checksum = calculateDataChecksum();

    bool ok = Drivers::EERAM::write(
        Config::EeramAddresses::SettingsBase,
        reinterpret_cast<const uint8_t*>(&data),
        sizeof data
    );

    if (ok) {
        if (!Drivers::EERAM::getStatus().ase) {
            _log.debug("setting ASE bit");
            Drivers::EERAM::setAseEnabled(true);
        }
    } else {
        _log.error("failed to write settings to EERAM");
    }
}

uint32_t Settings::calculateDataChecksum() const
{
    return crc32(
        reinterpret_cast<const uint8_t*>(&data) + sizeof data.checksum,
        sizeof data - sizeof data.checksum
    );
}

bool Settings::initializeScheduleDataHeader() const
{
    SchedulerDataHeader header;

    return writeScheduleDataHeader(header);
}

bool Settings::readScheduleDataHeader(SchedulerDataHeader& header) const
{
    auto ok = Drivers::EERAM::read(
        Config::EeramAddresses::SchedulerDataHeaderBase,
        reinterpret_cast<uint8_t*>(&header),
        sizeof header
    );

    ok &= header.checksum == crc32(
        reinterpret_cast<uint8_t*>(&header) + sizeof header.checksum,
        sizeof header - sizeof header.checksum
    );

    return ok;
}

bool Settings::writeScheduleDataHeader(SchedulerDataHeader& header) const
{
    header.checksum = crc32(
        reinterpret_cast<uint8_t*>(&header) + sizeof header.checksum,
        sizeof header - sizeof header.checksum
    );

    const auto ok = Drivers::EERAM::write(
        Config::EeramAddresses::SchedulerDataHeaderBase,
        reinterpret_cast<uint8_t*>(&header),
        sizeof header
    );

    return ok;
}

bool Settings::readSchedulerDataItem(const SchedulerDataHeader& header, uint8_t idx, SchedulerDataItem& item) const
{
    if (idx >= header.count) {
        return false;
    }

    auto ok = Drivers::EERAM::read(
        Config::EeramAddresses::SchedulerDataBase + idx * sizeof item,
        reinterpret_cast<uint8_t*>(&item),
        sizeof item
    );

    if (!ok || item.erased) {
        return false;
    }

    ok = item.checksum == crc32(
        reinterpret_cast<uint8_t*>(&item) + sizeof item.checksum,
        sizeof item - sizeof item.checksum
    );

    return ok;
}

bool Settings::writeSchedulerDataItem(const SchedulerDataHeader& header, uint8_t idx, const SchedulerDataItem& item) const
{
    if (idx >= header.count) {
        return false;
    }

    auto ok = Drivers::EERAM::write(
        Config::EeramAddresses::SchedulerDataBase + idx * sizeof item,
        reinterpret_cast<const uint8_t*>(&item),
        sizeof item
    );

    return ok;
}

bool Settings::addSchedulerDataItem(SchedulerDataHeader& header, const SchedulerDataItem& item, uint8_t& newIndex) const
{
    uint8_t idx = 0;
    auto overwritten = false;

    while (idx < header.count)
    {
        SchedulerDataItem i;

        // Damaged items can be overwritten
        if (!readSchedulerDataItem(header, idx, i)) {
            overwritten = true;
            break;
        }

        if (i.erased) {
            overwritten = true;
            break;
        }

        ++idx;

        // Index overflow, storage is full
        if (idx == 0) {
            return false;
        }
    }

    newIndex = idx;

    if (!overwritten) {
        ++header.count;
        auto ok = writeScheduleDataHeader(header);
        if (!ok) {
            return false;
        }
    }

    auto ok = writeSchedulerDataItem(header, newIndex, item);

    return ok;
}

bool Settings::eraseSchedulerDataItem(SchedulerDataHeader& header, const uint8_t idx) const
{
    if (idx >= header.count) {
        return false;
    }

    SchedulerDataItem item;
    item.erased = 1;
    item.checksum = crc32(
        reinterpret_cast<uint8_t*>(&item) + sizeof item.checksum,
        sizeof item - sizeof item.checksum
    );

    auto ok = Drivers::EERAM::write(
        Config::EeramAddresses::SchedulerDataBase + idx * sizeof item,
        reinterpret_cast<const uint8_t*>(&item),
        sizeof item
    );

    if (idx + 1 == header.count) {
        --header.count;
        writeScheduleDataHeader(header);
    }

    return ok;
}
