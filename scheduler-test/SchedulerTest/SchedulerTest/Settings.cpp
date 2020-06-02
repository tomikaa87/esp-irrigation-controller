#include "Settings.h"

#include <fstream>
#include <iostream>

void Settings::loadDefaults()
{
    data.tank.capacity = 360;
    data.tank.level = 100;

    data.flowSensor.ticksPerDecilitres = 100;
    data.flowSensor.errorDetectionTicks = 1;
}

void Settings::load()
{
    std::fstream fs{ "settings.bin", std::ios::in | std::ios::binary };
    if (fs.is_open())
    {
        fs.get(reinterpret_cast<char*>(&data), sizeof data);
    }
    else
    {
        loadDefaults();
        save();
    }

    std::cout << "settings loaded: " << data << std::endl;
}

void Settings::save() const
{
    std::cout << "saving settings: " << data << std::endl;

    std::fstream fs{ "settings.bin", std::ios::out | std::ios::binary | std::ios::trunc };
    if (fs.is_open())
    {
        fs.write(reinterpret_cast<const char*>(&data), sizeof data);
        fs.flush();
    }
}
