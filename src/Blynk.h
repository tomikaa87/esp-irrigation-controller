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
    Created on 2020-08-09
*/

#pragma once

#include "Types.h"

#include <Logger.h>
#include <Variant.h>

#include <array>
#include <cstdlib>
#include <functional>
#include <vector>

class IBlynkHandler;
class Settings;

class Blynk
{
public:
    explicit Blynk(IBlynkHandler& handler, Settings& settings);

    void task();

    struct StartedZone {
        Decilitres amount = 0;
        uint8_t zone = 0;
    };

    using StartHandler = std::function<void (const std::vector<StartedZone>& zones)>;
    using StopHandler = std::function<void ()>;

    void setStartHandler(StartHandler&& handler);
    void setStopHandler(StopHandler&& handler);

    void setControlsEnabled(bool enabled);
    void setFlowSensorTicks(size_t count);
    void setStatusText(const std::string& text);

    void resetSelectors();

private:
    Logger _log{ "Blynk" };
    IBlynkHandler& _handler;
    Settings& _settings;

    struct Zone {
        bool selected = false;
    };

    std::array<Zone, Config::Zones> _zones;
    std::string _lastStatusText;
    size_t _lastFlowSensorTicks = 0;
    bool _controlsEnabled = true;

    bool _settingsChanged = false;
    int32_t _settingsSaveTimer = 0;

    StartHandler _startHandler;
    StopHandler _stopHandler;

    void setupHandlers();

    void onConnected();
    Variant onPinRead(int pin);
    void onPinWritten(int pin, const Variant& value);
    void onStartButtonPressed();
    void onStopButtonPressed();

    void saveSettingsDeferred();

    static size_t virtualPinToZoneIndex(int pin);
};