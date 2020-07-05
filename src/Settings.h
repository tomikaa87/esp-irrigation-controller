#pragma once

#include "Logger.h"
#include "Types.h"

#include <ostream>

#define SETTINGS_PACKED __attribute__((packed))

class Settings
{
public:
    struct TankSettings
    {
        Decilitres capacity = 0;
        Decilitres level = 0;
    } SETTINGS_PACKED;

    struct FlowSensorSettings
    {
        uint16_t ticksPerDecilitres = 0;
        uint16_t errorDetectionTicks = 0;
        uint16_t leakCheckDetectionTicks = 0;
    } SETTINGS_PACKED;

    struct Data
    {
        uint32_t checksum = 0;

        TankSettings tank;
        FlowSensorSettings flowSensor;
    } SETTINGS_PACKED data;

    struct SchedulerDataHeader
    {
        uint32_t checksum = 0;
        uint8_t count = 0;
    } SETTINGS_PACKED;

    struct SchedulerDataItem
    {
        uint32_t checksum = 0;

        uint8_t active : 1;
        uint8_t erased : 1;
        uint8_t : 0;

        uint8_t days : 7;
        Decilitres amount = 0;
        MinutesFromMidnight time = 0;
        unsigned long zones = 0;

        SchedulerDataItem()
            : active(0)
            , erased(0)
            , days(0)
        {}
    } SETTINGS_PACKED;

    Settings();

    void loadDefaults();
    void load();
    void save();

    bool initializeScheduleDataHeader() const;
    bool readScheduleDataHeader(SchedulerDataHeader& header) const;
    bool writeScheduleDataHeader(SchedulerDataHeader& header) const;

    bool readSchedulerDataItem(const SchedulerDataHeader& header, uint8_t idx, SchedulerDataItem& item) const;
    bool writeSchedulerDataItem(const SchedulerDataHeader& header, uint8_t idx, const SchedulerDataItem& item) const;
    bool addSchedulerDataItem(SchedulerDataHeader& header, const SchedulerDataItem& item, uint8_t& newIndex) const;
    bool eraseSchedulerDataItem(SchedulerDataHeader& header, uint8_t idx) const;

private:
    Logger _log{ "Settings" };

    uint32_t calculateDataChecksum() const;
};

inline std::ostream& operator<<(std::ostream& s, const Settings::Data& d)
{
    s << "Settings::Data{" << std::endl
        << "  tank.capacity=" << d.tank.capacity << std::endl
        << "  tank.level=" << d.tank.level << std::endl
        << "  flowSensor.ticksPerDecilitres=" << d.flowSensor.ticksPerDecilitres << std::endl
        << "}";

    return s;
}