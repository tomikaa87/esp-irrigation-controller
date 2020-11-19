#include "Config.h"
#include "OutputController.h"
#include "ZoneController.h"

ZoneController::ZoneController(OutputController& outputController)
    : _outputController(outputController)
{}

void ZoneController::open(const uint8_t zone)
{
    _log.info("opening zone: zone=%u", zone);

    _outputController.activate(Config::Pins::ZoneOutputBase + zone);
}

void ZoneController::close(const uint8_t zone)
{
    _log.info("closing zone: zone=%u", zone);

    _outputController.deactive(Config::Pins::ZoneOutputBase + zone);
}

void ZoneController::openAll()
{
    _log.info("opening all zones");

    for (auto i = 0u; i < Config::Zones; ++i) {
        _outputController.activate(Config::Pins::ZoneOutputBase + i);
    }
}

void ZoneController::closeAll()
{
    _log.info("closing all zones");

    for (auto i = 0u; i < Config::Zones; ++i) {
        _outputController.deactive(Config::Pins::ZoneOutputBase + i);
    }
}
