#include "Config.h"
#include "OutputController.h"
#include "PumpController.h"

#include <iostream>

PumpController::PumpController(OutputController& outputController)
    : _outputController(outputController)
{}

void PumpController::start(const uint8_t pump)
{
    if (pump >= Config::Pumps) {
        _log.error("cannot start invalid pump: pump=%u", pump);
        return;
    }

    _log.info("starting, pump=%u", pump);

    _running[pump] = true;
    _outputController.activate(Config::Pins::PumpOutputBase + pump);
}

void PumpController::stop(const uint8_t pump)
{
    if (pump >= Config::Pumps) {
        _log.error("cannot stop invalid pump: pump=%u", pump);
        return;
    }

    _log.info("stopping, pump=%u", pump);

    _running[pump] = false;
    _outputController.deactive(Config::Pins::PumpOutputBase + pump);
}

bool PumpController::isRunning(const uint8_t pump) const
{
    if (pump >= Config::Pumps) {
        _log.error("cannot get state of invalid pump: pump=%u", pump);
        return false;
    }

    return _running[pump];
}
