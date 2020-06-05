#pragma once

#include <Arduino.h>

namespace Config
{
    static constexpr auto Zones = 6u;

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
        static constexpr auto MCP23008 = 0b00100000u; // [A2..A0] = 0b000
    }

    namespace Pins
    {
        // WeMos D1 Pins
        static constexpr auto FlowSensorInput = D0;

        // MCP23008 Pins
        static constexpr auto Zone0RelayOutput = 0;
        static constexpr auto Zone1RelayOutput = 1;
        static constexpr auto Zone2RelayOutput = 2;
        static constexpr auto Zone3RelayOutput = 3;
        static constexpr auto Zone4RelayOutput = 4;
        static constexpr auto Zone5RelayOutput = 5;
        static constexpr auto GreenLedOutput = 6;
        static constexpr auto RedLedOutput = 7;
    }
}
