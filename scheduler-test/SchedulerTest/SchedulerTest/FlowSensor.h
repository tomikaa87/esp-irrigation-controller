#pragma once

#include <cstdlib>

class FlowSensor
{
public:
    std::size_t ticks() const;
    void reset();

    void task();

private:
    std::size_t _ticks = 0;
};

