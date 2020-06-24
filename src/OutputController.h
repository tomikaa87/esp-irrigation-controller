#pragma once

#include "Logger.h"

class OutputController
{
public:
    OutputController();

    void activate(uint8_t output);
    void deactive(uint8_t output);

private:
    Logger _log{ "OutputController" };
};