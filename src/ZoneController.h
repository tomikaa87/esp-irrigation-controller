#pragma once

#include "Logger.h"

#include <cstdint>

class OutputController;

class ZoneController
{
public:
    explicit ZoneController(OutputController& outputController);

    void open(uint8_t zone);
    void close(uint8_t zone);

    void openAll();
    void closeAll();

private:
    Logger _log{ "ZoneController" };
    OutputController& _outputController;
};

