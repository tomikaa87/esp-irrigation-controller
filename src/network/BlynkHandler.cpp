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

#include "BlynkHandler.h"

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define BLYNK_PRINT     Serial
#endif // ENABLE_DEBUG

#include <BlynkSimpleEsp8266.h>

static BlynkHandler* g_blynkHandler = nullptr;

#define HANDLE_BLYNK_WRITE(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->onVirtualPinUpdated(__PIN, param); \
}

#define HANDLE_BLYNK_READ(__PIN) BLYNK_READ(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->updateVirtualPin(__PIN); \
}

#define HANDLE_BLYNK_BUTTON_PRESS(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_blynkHandler && param.asInt() == 1) \
        g_blynkHandler->onButtonPressed(__PIN); \
}

BLYNK_CONNECTED()
{
    if (g_blynkHandler)
        g_blynkHandler->onBlynkConnected();
}

#define PIN_ZONE0_AMOUNT                V10
#define PIN_ZONE0_SELECT                V16
#define PIN_ZONE1_AMOUNT                V11
#define PIN_ZONE1_SELECT                V17
#define PIN_ZONE2_AMOUNT                V12
#define PIN_ZONE2_SELECT                V18
#define PIN_ZONE3_AMOUNT                V13
#define PIN_ZONE3_SELECT                V19
#define PIN_ZONE4_AMOUNT                V14
#define PIN_ZONE4_SELECT                V20
#define PIN_ZONE5_AMOUNT                V15
#define PIN_ZONE5_SELECT                V21
#define PIN_START_BUTTON                V22
#define PIN_STOP_BUTTON                 V23
#define PIN_STATUS_LABEL                V24
#define PIN_FLS_TICKS_LABEL             V25

HANDLE_BLYNK_BUTTON_PRESS(PIN_START_BUTTON)
HANDLE_BLYNK_BUTTON_PRESS(PIN_STOP_BUTTON)

HANDLE_BLYNK_WRITE(PIN_ZONE0_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE1_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE2_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE3_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE4_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE5_AMOUNT)
HANDLE_BLYNK_WRITE(PIN_ZONE0_SELECT)
HANDLE_BLYNK_WRITE(PIN_ZONE1_SELECT)
HANDLE_BLYNK_WRITE(PIN_ZONE2_SELECT)
HANDLE_BLYNK_WRITE(PIN_ZONE3_SELECT)
HANDLE_BLYNK_WRITE(PIN_ZONE4_SELECT)
HANDLE_BLYNK_WRITE(PIN_ZONE5_SELECT)

HANDLE_BLYNK_READ(PIN_ZONE0_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE1_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE2_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE3_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE4_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE5_AMOUNT)
HANDLE_BLYNK_READ(PIN_ZONE0_SELECT)
HANDLE_BLYNK_READ(PIN_ZONE1_SELECT)
HANDLE_BLYNK_READ(PIN_ZONE2_SELECT)
HANDLE_BLYNK_READ(PIN_ZONE3_SELECT)
HANDLE_BLYNK_READ(PIN_ZONE4_SELECT)
HANDLE_BLYNK_READ(PIN_ZONE5_SELECT)


static WidgetTerminal gs_terminal{ V64 };

BlynkHandler::BlynkHandler(const char* appToken)
{
    g_blynkHandler = this;
    Blynk.config(appToken, "blynk-server.home", 8080);
}

BlynkHandler::~BlynkHandler()
{
    g_blynkHandler = nullptr;
}

void BlynkHandler::task()
{
    Blynk.run();
}

void BlynkHandler::onBlynkConnected()
{
    _log.debug("connected");

    resetSelectors();
}

void BlynkHandler::onVirtualPinUpdated(int pin, const BlynkParam& param)
{
    _log.debug("virtual pin updated, pin: %d", pin);

    // Handle "real" data updates (not button presses)
    // here.

    // This callback should run as short as possible
    // to avoid Blynk disconnect errors

    if (pin >= PIN_ZONE0_AMOUNT && pin <= PIN_ZONE5_AMOUNT) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        // Value is clamped
        zone.amount = std::min(100, std::max(static_cast<Decilitres>(param.asFloat() * 10), 0));
        _log.debug("amount changed, zone=%u, amount=%u", zoneIdx, zone.amount);
    } else if (pin >= PIN_ZONE0_SELECT && pin <= PIN_ZONE5_SELECT) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        zone.selected = param.asInt() > 0;
        _log.debug("selection changed, zone=%u, selected=%u", zoneIdx, zone.selected);
    }
}

void BlynkHandler::onButtonPressed(int pin)
{
    // This callback should run as short as possible
    // to avoid Blynk disconnect errors

    switch (pin) {
        case PIN_START_BUTTON:
            onStartButtonPressed();
            break;

        case PIN_STOP_BUTTON:
            onStopButtonPressed();
            break;

        default:
            break;
    }
}

void BlynkHandler::updateVirtualPin(int pin)
{
    _log.debug("updating virtual pin: %d", pin);

    if (pin >= PIN_ZONE0_AMOUNT && pin <= PIN_ZONE5_AMOUNT) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        Blynk.virtualWrite(pin, zone.amount);
        return;
    }

    if (pin >= PIN_ZONE0_SELECT && pin <= PIN_ZONE5_SELECT) {
        const auto zoneIdx = virtualPinToZoneIndex(pin);
        auto& zone = _zones[zoneIdx];
        Blynk.virtualWrite(pin, zone.selected ? 1 : 0);
        return;
    }

    switch (pin) {
        case PIN_STATUS_LABEL:
            Blynk.virtualWrite(pin, _lastStatusText.c_str());
            break;

        case PIN_FLS_TICKS_LABEL:
            Blynk.virtualWrite(pin, _lastFlowSensorTicks);
            break;

        default:
            break;
    }
}

void BlynkHandler::setStatusText(const std::string& s)
{
    Blynk.virtualWrite(PIN_STATUS_LABEL, s.c_str());
    _lastStatusText = s;
}

void BlynkHandler::setFlowSensorTicks(const size_t ticks)
{
    Blynk.virtualWrite(PIN_FLS_TICKS_LABEL, ticks);
    _lastFlowSensorTicks = ticks;
}

void BlynkHandler::setControlsEnabled(const bool enabled)
{
    _log.debug("setting controls %s", enabled ? "enabled" : "disabled");
    _controlsEnabled = enabled;
}

void BlynkHandler::resetSelectors()
{
    _log.debug("resetting zone selectors");

    for (auto pin = PIN_ZONE0_SELECT, zone = 0; pin <= PIN_ZONE5_SELECT; ++pin, ++zone) {
        Blynk.virtualWrite(pin, 0);
        _zones[zone].selected = 0;
    }

    for (auto pin = PIN_ZONE0_AMOUNT, zone = 0; pin <= PIN_ZONE5_AMOUNT; ++pin, ++zone) {
        Blynk.virtualWrite(pin, _zones[zone].amount / 10.0);
    }
}

void BlynkHandler::terminalPrintln(const char* msg)
{
    gs_terminal.println(msg);
    gs_terminal.flush();
}

void BlynkHandler::setStartHandler(StartHandler&& handler)
{
    _startHandler = std::move(handler);
}

void BlynkHandler::setStopHandler(StopHandler&& handler)
{
    _stopHandler = std::move(handler);
}

size_t BlynkHandler::virtualPinToZoneIndex(const int pin)
{
    if (pin >= PIN_ZONE0_AMOUNT && pin <= PIN_ZONE5_AMOUNT) {
        return pin - PIN_ZONE0_AMOUNT;
    }

    if (pin >= PIN_ZONE0_SELECT && pin <= PIN_ZONE5_SELECT) {
        return pin - PIN_ZONE0_SELECT;
    }

    return 0;
}

void BlynkHandler::onStartButtonPressed()
{
    if (!_controlsEnabled) {
        return;
    }

    _log.debug("START button pressed");

    std::vector<StartedZone> zones;

    for (auto i = 0u; i < _zones.size(); ++i) {
        auto& zone = _zones[i];

        if (!zone.selected) {
            continue;
        }

        zone.selected = false;

        _log.debug("starting selected zone, index=%u, amount=%u", i, zone.amount);

        StartedZone sz;
        sz.amount = zone.amount;
        sz.zone = i;

        zones.push_back(std::move(sz));
    }

    if (zones.empty()) {
        _log.debug("no zone selected to start");
        return;
    }

    if (!_startHandler) {
        _log.warning("startHandler is empty");
        return;
    }

    _startHandler(zones);
}

void BlynkHandler::onStopButtonPressed()
{
    _log.debug("STOP button pressed");

    if (!_stopHandler) {
        _log.warning("stopHandler is empty");
        return;
    }

    _stopHandler();
}

template <typename T, int size>
inline void BlynkHandler::floatToStr(const float f, T(&buf)[size])
{
    static_assert(size >= 4, "Output buffer is too small");
    snprintf(buf, size - 1, "%0.1f", f);
}
