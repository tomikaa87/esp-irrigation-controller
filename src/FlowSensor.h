#pragma once

#include "Logger.h"

#include <cstdlib>

class FlowSensor
{
public:
    FlowSensor();
    ~FlowSensor();

    std::size_t ticks() const;
    void reset();

    void task();

    ICACHE_RAM_ATTR void onIoInterruptCalled();

private:
    Logger _log{ "FlowSensor" };
    std::size_t _ticks = 0;
};

