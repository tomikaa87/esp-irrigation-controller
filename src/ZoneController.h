#pragma once

#include "Logger.h"

#include <cstdint>

class ZoneController
{
public:
    void open(uint8_t zone);
    void closeAll();

private:
    Logger _log{ "ZoneController" };
};

