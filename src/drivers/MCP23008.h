#pragma once

#include "Config.h"
#include "Logger.h"

#include <cstdint>

namespace Drivers
{

class MCP23008
{
public:
    static constexpr auto SlaveAddress = 0b0100000 | Config::SlaveAddresses::MCP23008;

    enum class Register : uint8_t
    {
        IODIR,
        IPOL,
        GPINTEN,
        DEFVAL,
        INTCON,
        IOCON,
        GPPU,
        INTF,
        INTCAP,
        GPIO,
        OLAT
    };

    MCP23008() = delete;

    static bool init();

    static bool writeRegister(Register reg, uint8_t value);
    static bool readRegister(Register reg, uint8_t& value);

private:
    static Logger _log;
};

}