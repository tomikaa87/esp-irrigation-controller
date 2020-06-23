#pragma once

#include "Config.h"
#include "Logger.h"

class OutputController;

class PumpController
{
public:
    explicit PumpController(OutputController& outputController);

    void start(uint8_t pump);
    void stop(uint8_t pump);
    bool isRunning(uint8_t pump) const;

private:
    Logger _log{ "PumpController" };
    OutputController _outputController;
    bool _running[Config::Pumps] = { 0 };
};

