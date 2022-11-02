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

#ifdef IOT_ENABLE_BLYNK

#include "Blynk.h"
#include "Settings.h"

#include <IBlynkHandler.h>

namespace VirtualPins
{
    namespace Amount
    {
        static constexpr auto Zone0 = 10;
        static constexpr auto Zone1 = 11;
        static constexpr auto Zone2 = 12;
        static constexpr auto Zone3 = 13;
        static constexpr auto Zone4 = 14;
        static constexpr auto Zone5 = 15;
    }

    namespace Select
    {
        static constexpr auto Zone0 = 16;
        static constexpr auto Zone1 = 17;
        static constexpr auto Zone2 = 18;
        static constexpr auto Zone3 = 19;
        static constexpr auto Zone4 = 20;
        static constexpr auto Zone5 = 21;
    }

    namespace Buttons
    {
        static constexpr auto Start = 22;
        static constexpr auto Stop = 23;
    }

    namespace Labels
    {
        static constexpr auto Status = 24;
        static constexpr auto FlowSensorTicks = 25;
    }
}

Blynk::Blynk(IBlynkHandler& handler, Settings& settings)
    : _handler(handler)
    , _settings(settings)
{
    setupHandlers();
}

void Blynk::task()
{
    if (_settingsChanged && millis() - _settingsSaveTimer > Config::Blynk::SettingsSaveTimeoutMs) {
        _settingsChanged = false;
        _settings.save();
    }
}

void Blynk::setStartHandler(StartHandler&& handler)
{
    _startHandler = std::move(handler);
}

void Blynk::setStopHandler(StopHandler&& handler)
{
    _stopHandler = std::move(handler);
}

void Blynk::setControlsEnabled(const bool enabled)
{
    _log.debug_P(PSTR("setting controls %s", enabled ? "enabled" : "disabled"));
    _controlsEnabled = enabled;
}

void Blynk::setFlowSensorTicks(const size_t count)
{
    _handler.writePin(
        VirtualPins::Labels::FlowSensorTicks,
        Variant{ static_cast<int>(count) }
    );

    _lastFlowSensorTicks = count;
}

void Blynk::setStatusText(const std::string& text)
{
    _lastStatusText = text;

    _handler.writePin(
        VirtualPins::Labels::Status,
        Variant{ _lastStatusText.c_str() }
    );
}

void Blynk::resetSelectors()
{
    _log.debug_P(PSTR("resetting zone selectors"));

    for (auto pin = VirtualPins::Select::Zone0, zone = 0; pin <= VirtualPins::Select::Zone5; ++pin, ++zone) {
        _handler.writePin(pin, Variant{ 0 });
        _zones[zone].selected = 0;
    }

    for (auto pin = VirtualPins::Amount::Zone0, zone = 0; pin <= VirtualPins::Amount::Zone5; ++pin, ++zone) {
        _handler.writePin(pin, Variant{ _settings.data.irrigation.amounts[zone] / 10.0 });
    }
}

void Blynk::setupHandlers()
{
    _handler.setConnectedHandler([this] {
        onConnected();
    });

    _handler.setPinReadHandler([this](const int pin) {
        return onPinRead(pin);
    });

    _handler.setPinWrittenHandler([this](const int pin, const Variant& value) {
        onPinWritten(pin, value);
    });

    _handler.setPinWrittenHandler(
        VirtualPins::Buttons::Start,
        [this](int, const Variant& value) {
            if (value == 1) {
                onStartButtonPressed();
            }
        }
    );

    _handler.setPinWrittenHandler(
        VirtualPins::Buttons::Stop,
        [this](int, const Variant& value) {
            if (value == 1) {
                onStopButtonPressed();
            }
        }
    );
}

void Blynk::onConnected()
{
    resetSelectors();
}

Variant Blynk::onPinRead(const int pin)
{
    if (pin >= VirtualPins::Amount::Zone0 && pin <= VirtualPins::Amount::Zone5) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        return Variant{ _settings.data.irrigation.amounts[zoneIdx] };
    }

    if (pin >= VirtualPins::Select::Zone0 && pin <= VirtualPins::Select::Zone5) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        return Variant{ zone.selected ? 1 : 0 };
    }

    switch (pin) {
        case VirtualPins::Labels::Status:
            // FIXME guarantee that the string remains valid until it's written
            return Variant{ _lastStatusText.c_str() };

        case VirtualPins::Labels::FlowSensorTicks:
            return Variant{ static_cast<int>(_lastFlowSensorTicks) };

        default:
            break;
    }

    return {};
}

void Blynk::onPinWritten(const int pin, const Variant& value)
{
    if (pin >= VirtualPins::Amount::Zone0 && pin <= VirtualPins::Amount::Zone5) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);

        // Value is clamped
        const auto amount = std::min(
            100,
            std::max(
                static_cast<Decilitres>(static_cast<double>(value) * 10),
                0
            )
        );

        _log.debug_P(PSTR("amount changed, zone=%u, amount=%u"), zoneIdx, amount);
        _settings.data.irrigation.amounts[zoneIdx] = amount;
        saveSettingsDeferred();
    } else if (pin >= VirtualPins::Select::Zone0 && pin <= VirtualPins::Select::Zone5) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        zone.selected = static_cast<int>(value) > 0;
        _log.debug_P(PSTR("selection changed, zone=%u, selected=%u"), zoneIdx, zone.selected);
    }

}

void Blynk::onStartButtonPressed()
{
    _log.debug_P(PSTR("START button pressed"));

    std::vector<StartedZone> zones;

    for (auto i = 0u; i < _zones.size(); ++i) {
        auto& zone = _zones[i];

        if (!zone.selected) {
            continue;
        }

        zone.selected = false;

        const auto amount = _settings.data.irrigation.amounts[i];

        _log.debug_P(PSTR("starting selected zone, index=%u, amount=%u"), i, amount);

        StartedZone sz;
        sz.amount = amount;
        sz.zone = i;

        zones.push_back(std::move(sz));
    }

    if (zones.empty()) {
        _log.debug_P(PSTR("no zone selected to start"));
        return;
    }

    if (!_startHandler) {
        _log.warning_P(PSTR("startHandler is empty"));
        return;
    }

    _startHandler(zones);
}

void Blynk::onStopButtonPressed()
{
    _log.debug_P(PSTR("STOP button pressed"));

    if (!_stopHandler) {
        _log.warning_P(PSTR("stopHandler is empty"));
        return;
    }

    _stopHandler();
}

void Blynk::saveSettingsDeferred()
{
    _settingsChanged = true;
    _settingsSaveTimer = millis();
}

size_t Blynk::virtualPinToZoneIndex(const int pin)
{
    if (pin >= VirtualPins::Amount::Zone0 && pin <= VirtualPins::Amount::Zone5) {
        return pin - VirtualPins::Amount::Zone0;
    }

    if (pin >= VirtualPins::Select::Zone0 && pin <= VirtualPins::Select::Zone5) {
        return pin - VirtualPins::Select::Zone0;
    }

    return 0;
}

#endif