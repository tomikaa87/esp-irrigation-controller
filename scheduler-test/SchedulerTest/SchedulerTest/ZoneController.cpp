#include "ZoneController.h"

#include <iostream>

void ZoneController::open(uint8_t zone)
{
    std::cout << "opening zone: " << static_cast<int>(zone) << std::endl;
}

void ZoneController::closeAll()
{
    std::cout << "closing all zones" << std::endl;
}
