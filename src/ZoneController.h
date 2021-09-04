#pragma once

#include "Logger.h"

#include <cstdint>
#include <functional>
#include <vector>

class OutputController;

class ZoneController
{
public:
    explicit ZoneController(OutputController& outputController);

    void open(uint8_t zone);
    void close(uint8_t zone);

    void openAll();
    void closeAll();

    using ZoneChangedHandler = std::function<void (uint8_t zone, bool open)>;

    void addZoneChangedHandler(ZoneChangedHandler&& handler);

private:
    Logger _log{ "ZoneController" };
    OutputController& _outputController;
    std::vector<ZoneChangedHandler> _changedHandlers;

    void notifyChangedHandlers(uint8_t zone, bool open);
};

