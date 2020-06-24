#include "MCP23008.h"
#include "SimpleI2C.h"

using namespace Drivers;

Logger MCP23008::_log = Logger{ "MCP23008" };

bool MCP23008::init()
{
    _log.info("initializing");

    if (!I2C::start(SlaveAddress, I2C::Operation::Write)) {
        _log.error("init() write error: cannot start transfer");
        return false;
    }

    if (!I2C::write(static_cast<uint8_t>(Register::IODIR))) {
        _log.error("init() write error: cannot write register address");
        return false;
    }

    static const uint8_t DefaultValues[] = {
        0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    // Reset all registers to default values
    if (!I2C::write(DefaultValues, sizeof(DefaultValues))) {
        _log.error("init() write error: cannot write IODIR data");
        return false;
    }

    I2C::end();

    return true;
}

bool MCP23008::writeRegister(const Register reg, const uint8_t value)
{
    if (!I2C::start(SlaveAddress, I2C::Operation::Write)) {
        _log.error("writeRegister() write error: cannot start transfer");
        return false;
    }

    if (!I2C::write(static_cast<uint8_t>(reg))) {
        _log.error("writeRegister() write error: cannot write register address");
        return false;
    }

    if (!I2C::write(&value, 1)) {
        _log.error("writeRegister() register write error: cannot write value");
        return false;
    }

    I2C::end();

    return true;
}

bool MCP23008::readRegister(const Register reg, uint8_t& value)
{
    if (!I2C::write(MCP23008::SlaveAddress, reinterpret_cast<const uint8_t*>(&reg), sizeof(reg))) {
        _log.error("readRegister() read error: cannot start transfer");
        return false;
    }

    if (!I2C::read(MCP23008::SlaveAddress, &value, sizeof(value))) {
        _log.error("readRegister() read error: cannot read data");
        return false;
    }

    return true;
}