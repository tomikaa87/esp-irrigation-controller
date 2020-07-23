#pragma once

/*

PrivateConfig.h must contain the following:

#pragma once

namespace Config
{
    static constexpr auto BlynkAppToken = "<your Blynk application token";
    static constexpr auto WiFiSSID = "<SSID of you access point>";
    static constexpr auto WiFiPassword = "<password of your access point>";

    namespace Network
    {
        static constexpr auto FirmwareUpdateUrl = "<URL to your update server>";

        static constexpr auto SyslogServerHost = "<host name of your Syslog server>";
        static constexpr auto SyslogServerPort = <port of your Syslog server, 514 by default>;
        static constexpr auto SyslogHostName = "<host name for the current device, e.g. espirrigctl>";
    }
}

*/

#include "PrivateConfig.h"

#include <Arduino.h>

namespace Config
{
    static constexpr auto Zones = 6u;

    // Temporary
    static constexpr auto FlowSensorTicksPerDecilitre = 46; // 463/l

    namespace Network
    {
        static constexpr auto WebServerPort = 80;
        static constexpr auto SyslogEnabled = true;
    }

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
