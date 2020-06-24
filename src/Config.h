#pragma once

#include "PrivateConfig.h"

#include <Arduino.h>

namespace Config
{
    static constexpr auto Zones = 6u;
    static constexpr auto Pumps = 1u;

    namespace EeramAddresses
    {
        static constexpr auto EeramSize = 16384u;

        static constexpr auto SettingsBase = 0u;

        static constexpr auto SchedulerDataHeaderBase = 0x80u;
        static constexpr auto SchedulerDataBase = 0x90u;

        static constexpr auto SchedulerDataHeaderSize = SchedulerDataBase - SchedulerDataHeaderBase;
        static constexpr auto SchedulerDataSize = EeramSize - SchedulerDataBase;
    }

    namespace SlaveAddresses
    {
        static constexpr auto MCP23008 = 0b000; // [A2..A0] = 0b000
    }

    namespace Pins
    {
        // WeMos D1 Pins
        static constexpr auto FlowSensorInput = D0;

        // MCP23008 Pins
        static constexpr auto ZoneOutputBase = 0;
        static constexpr auto PumpOutputBase = 6;
    }
}
