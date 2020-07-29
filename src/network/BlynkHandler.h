/*
    This file is part of esp-irrigation-controller.

    esp-irrigation-controller is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    esp-irrigation-controller is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with esp-irrigation-controller.  If not, see <http://www.gnu.org/licenses/>.
    
    Author: Tamas Karpati
    Created on 2017-01-12
*/

#pragma once

#include "Logger.h"
#include "TrackedVariable.h"
#include "Types.h"

#include <cstdint>
#include <functional>

class BlynkParam;

class BlynkHandler
{
public:
    BlynkHandler(const char* appToken);
    ~BlynkHandler();

    void task();

    void onBlynkConnected();
    void onVirtualPinUpdated(int pin, const BlynkParam& param);
    void onButtonPressed(int pin);
    void updateVirtualPin(int pin);

    void setStatusText(const std::string& s);
    void setFlowSensorTicks(size_t ticks);

    void setControlsEnabled(bool enabled);
    void resetSelectors();

    void terminalPrintln(const char* msg);

    struct StartedZone {
        Decilitres amount = 0;
        uint8_t zone = 0;
    };

    using StartHandler = std::function<void(const std::vector<StartedZone>& zones)>;
    using StopHandler = std::function<void()>;

    void setStartHandler(StartHandler&& handler);
    void setStopHandler(StopHandler&& handler);

private:
    Logger _log{ "Blynk" };

    struct Zone {
        bool selected = false;
        Decilitres amount = 0;
    };

    std::array<Zone, Config::Zones> _zones;
    std::string _lastStatusText;
    size_t _lastFlowSensorTicks = 0;
    bool _controlsEnabled = true;

    StartHandler _startHandler;
    StopHandler _stopHandler;

    size_t virtualPinToZoneIndex(const int pin);

    void onStartButtonPressed();
    void onStopButtonPressed();

    template <typename T, int size>
    inline void floatToStr(const float f, T(&buf)[size]);
};
