#include "Settings.h"
#include "WaterTank.h"


#include <algorithm>
#include <iostream>

WaterTank::WaterTank(Settings& settings)
    : _settings{ settings }
{}

Decilitres WaterTank::capacity() const
{
    return _settings.data.tank.capacity;
}

Decilitres WaterTank::level() const
{
    return _settings.data.tank.level;
}

void WaterTank::refill() const
{
    std::cout << "refilling water tank: capacity=" << _settings.data.tank.capacity << std::endl;

    _settings.data.tank.level = _settings.data.tank.capacity;
    _settings.save();
}

void WaterTank::use(const Decilitres amount) const
{
    _settings.data.tank.level = std::max(_settings.data.tank.level - amount, 0);
    _settings.save();

    std::cout << "decreasing water tank level:"
        << " amount=" << amount
        << ", capacity=" << _settings.data.tank.capacity
        << ", level=" << _settings.data.tank.level
        << std::endl;
}
