#pragma once

#include <cstdint>

class ZoneController
{
public:
    void open(uint8_t zone);
    void closeAll();
};

