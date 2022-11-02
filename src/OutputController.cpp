#include "Config.h"
#include "OutputController.h"

#include "drivers/MCP23008.h"

using ioExpander = Drivers::MCP23008;

OutputController::OutputController()
{
    _log.info_P(PSTR("setting up IO expander"));

    if (!ioExpander::init()) {
        _log.error_P(PSTR("failed to initialize MCP23008"));
        return;
    }

    if (!ioExpander::writeRegister(ioExpander::Register::IODIR, 0)) {
        _log.error_P(PSTR("failed to set IODIR"));
        return;
    }
}

void OutputController::activate(const uint8_t output)
{
    if (output >= 8) {
        _log.error_P(PSTR("cannot activate invalid output: output=%u"), output);
        return;
    }

    uint8_t gpio = 0;

    if (!ioExpander::readRegister(ioExpander::Register::GPIO, gpio)) {
        _log.error_P(PSTR("failed to read GPIO"));
        return;
    }

    gpio |= 1 << output;

    if (!ioExpander::writeRegister(ioExpander::Register::GPIO, gpio)) {
        _log.error_P(PSTR("failed to write GPIO"));
        return;
    }
}

void OutputController::deactive(const uint8_t output)
{
    if (output >= 8) {
        _log.error_P(PSTR("cannot deactivate invalid output: output=%u"), output);
        return;
    }

    uint8_t gpio = 0;

    if (!ioExpander::readRegister(ioExpander::Register::GPIO, gpio)) {
        _log.error_P(PSTR("failed to read GPIO"));
        return;
    }

    gpio &= ~(1 << output);

    if (!ioExpander::writeRegister(ioExpander::Register::GPIO, gpio)) {
        _log.error_P(PSTR("failed to write GPIO"));
        return;
    }
}
