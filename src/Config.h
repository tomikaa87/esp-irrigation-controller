#pragma once

#include <Arduino.h>

#include <cstdint>

namespace Config
{
    static constexpr auto Zones = 6u;
    static constexpr auto Pumps = 1u;

    namespace Blynk
    {
        static constexpr auto SettingsSaveTimeoutMs = 2000;
    }

    namespace SlaveAddresses
    {
        static constexpr auto MCP23008 = 0b000; // [A2..A0] = 0b000
    }

    namespace Pins
    {
        // WeMos D1 Pins
        static constexpr auto FlowSensorInput = D5;

        // MCP23008 Pins
        static constexpr auto ZoneOutputBase = 0;
        static constexpr auto PumpOutputBase = 6;
    }

    namespace Pump
    {
        static constexpr int32_t PumpRampUpTimeMs = 2000;
        static constexpr int32_t LeakCheckLengthMs = 5000;
        static constexpr int32_t FlowSensorCheckIntervalMs = 1000;
    }
}
