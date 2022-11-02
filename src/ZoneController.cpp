#include "Config.h"
#include "OutputController.h"
#include "ZoneController.h"

ZoneController::ZoneController(OutputController& outputController)
    : _outputController(outputController)
{}

void ZoneController::open(const uint8_t zone)
{
    _log.info_P(PSTR("opening zone: zone=%u"), zone);

    _outputController.activate(Config::Pins::ZoneOutputBase + zone);

    notifyChangedHandlers(zone, true);
}

void ZoneController::close(const uint8_t zone)
{
    _log.info_P(PSTR("closing zone: zone=%u"), zone);

    _outputController.deactive(Config::Pins::ZoneOutputBase + zone);

    notifyChangedHandlers(zone, false);
}

void ZoneController::openAll()
{
    _log.info_P(PSTR("opening all zones"));

    for (auto i = 0u; i < Config::Zones; ++i) {
        _outputController.activate(Config::Pins::ZoneOutputBase + i);
        notifyChangedHandlers(i, true);
    }
}

void ZoneController::closeAll()
{
    _log.info_P(PSTR("closing all zones"));

    for (auto i = 0u; i < Config::Zones; ++i) {
        _outputController.deactive(Config::Pins::ZoneOutputBase + i);
        notifyChangedHandlers(i, false);
    }
}

void ZoneController::addZoneChangedHandler(ZoneChangedHandler&& handler)
{
    _changedHandlers.emplace_back(std::move(handler));
}

void ZoneController::notifyChangedHandlers(const uint8_t zone, const bool open)
{
    for (const auto& h : _changedHandlers) {
        if (h) {
            h(zone, open);
        }
    }
}
