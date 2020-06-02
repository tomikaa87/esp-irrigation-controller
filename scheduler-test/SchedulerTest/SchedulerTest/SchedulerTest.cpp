#include "IrrigationController.h"

#include <thread>

int main(int, char**)
{
    IrrigationController irrigationController;

    while (true)
    {
        irrigationController.task();
        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
    }

    return 0;
}