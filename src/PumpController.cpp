#include "PumpController.h"

#include <iostream>

void PumpController::start()
{
    std::cout << "starting pump" << std::endl;
    _running = true;
}

void PumpController::stop()
{
    std::cout << "stop pumping" << std::endl;
    _running = false;
}

bool PumpController::isRunning() const
{
    return _running;
}
