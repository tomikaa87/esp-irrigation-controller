#include "Config.h"
#include "LedController.h"

#include <Arduino.h>

LedController::LedController()
{
#if 0
    _ledStates[Led::Green].pin = Config::Pins::GreenLedOutput;
    _ledStates[Led::Red].pin = Config::Pins::RedLedOutput;
#endif
}

void LedController::setMode(const Led led, const Mode mode)
{
    auto& ls = _ledStates[led];
    ls.blinking = mode == Mode::Blink;
#if 0
    _ioExpander.digitalWrite(ls.pin, mode == Mode::On ? HIGH : LOW);
#endif
}

void LedController::turnOff()
{
#if 0
    for (auto& ls : _ledStates) {
        _ioExpander.digitalWrite(ls.second.pin, LOW);
    }
#endif
}

void LedController::task()
{
#if 0
    if (millis() >= _blinkTimer + 500u) {
        _blinkTimer = millis();
        _blinkOn = !_blinkOn;

        for (auto& ls : _ledStates) {
            if (ls.second.blinking) {
                _ioExpander.digitalWrite(ls.second.pin, _blinkOn ? HIGH : LOW);
            }
        }
    }
#endif
}