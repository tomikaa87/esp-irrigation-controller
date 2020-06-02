#include "FlowSensor.h"

#include <exception>
#include <iostream>

std::size_t FlowSensor::ticks() const
{
    return _ticks;
}

void FlowSensor::reset()
{
    std::cout << "resetting flow sensor tick count" << std::endl;
}

void FlowSensor::task()
{
    if (_ticks < 300)
        _ticks += 20;
}
