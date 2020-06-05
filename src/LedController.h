#pragma once

#include "Logger.h"

#include <cstdint>
#include <map>

class Adafruit_MCP23008;

class LedController
{
public:
    explicit LedController(Adafruit_MCP23008& ioExpander);

    enum class Mode
    {
        Off,
        On,
        Blink
    };

    enum class Led
    {
        Green,
        Red
    };

    void setMode(Led led, Mode mode);
    void turnOff();

    void task();

private:
    Logger _log{ "LedController" };
    Adafruit_MCP23008& _ioExpander;

    struct LedState
    {
        bool blinking = false;
        int pin = -1;
    };

    bool _blinkOn = false;
    int32_t _blinkTimer = 0;
    std::map<Led, LedState> _ledStates;
};