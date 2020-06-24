#include "Config.h"
#include "OutputController.h"

#include "MCP23008.h"

using namespace Drivers;

OutputController::OutputController()
{
    _log.info("setting up IO expander");

    if (!MCP23008::init()) {
        _log.error("failed to initialize MCP23008");
        return;
    }

    if (!MCP23008::writeRegister(MCP23008::Register::IODIR, 0)) {
        _log.error("failed to set IODIR");
        return;
    }
}

void OutputController::activate(const uint8_t output)
{
    if (output >= 8) {
        _log.error("cannot activate invalid output: output=%u", output);
        return;
    }

    uint8_t gpio = 0;

    if (!MCP23008::readRegister(MCP23008::Register::GPIO, gpio)) {
        _log.error("failed to read GPIO");
        return;
    }

    gpio |= 1 << output;

    if (!MCP23008::writeRegister(MCP23008::Register::GPIO, gpio)) {
        _log.error("failed to write GPIO");
        return;
    }
}

void OutputController::deactive(const uint8_t output)
{
    if (output >= 8) {
        _log.error("cannot deactivate invalid output: output=%u", output);
        return;
    }

    uint8_t gpio = 0;

    if (!MCP23008::readRegister(MCP23008::Register::GPIO, gpio)) {
        _log.error("failed to read GPIO");
        return;
    }

    gpio &= ~(1 << output);

    if (!MCP23008::writeRegister(MCP23008::Register::GPIO, gpio)) {
        _log.error("failed to write GPIO");
        return;
    }
}
