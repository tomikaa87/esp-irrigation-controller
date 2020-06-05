#pragma once

#include "Logger.h"

class PumpController
{
public:
    void start();
    void stop();
    bool isRunning() const;

private:
    Logger _log{ "PumpController" };
    bool _running = false;
};

