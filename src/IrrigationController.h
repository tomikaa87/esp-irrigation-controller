#pragma once

#include "FlowSensor.h"
#include "LedController.h"
#include "Logger.h"
#include "OutputController.h"
#include "PumpController.h"
#include "Scheduler.h"
#include "Settings.h"
#include "SystemClock.h"
#include "WaterTank.h"
#include "WebServer.h"
#include "ZoneController.h"

#include "network/BlynkHandler.h"
#include "network/NtpClient.h"
#include "network/OtaUpdater.h"

class IrrigationController
{
public:
    IrrigationController();

    void task();
    void epochTimerIsr();

private:
    Logger _log{ "IrrigationController" };
    SystemClock _systemClock;
    NtpClient _ntpClient;
    BlynkHandler _blynk;
    OutputController _outputController;
    Settings _settings;
    FlowSensor _flowSensor;
    WaterTank _waterTank;
    PumpController _pumpController;
    ZoneController _zoneController;
    Scheduler _scheduler;
    LedController _ledController;
    WebServer _webServer;
    OtaUpdater _otaUpdater;

    int32_t _lastTaskCallMillis = 0;
    std::size_t _taskCallCount = 0;

    enum class State
    {
        Idle,
        Starting,
        Pumping,
        Stopping,
        Error
    } _state = State::Idle;

    Decilitres _requiredAmount = 0;
    uint8_t _activeZone = 0;
    std::size_t _lastFlowSensorTicks = 0;
    uint8_t _flowErrorCount = 0;
    bool _manualIrrigation = false;

    bool _updateChecked = false;
    uint32_t _updateCheckTimer = 0;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t _lastSlowLoopUpdate = 0;

    static constexpr auto BlynkUpdateIntervalMs = 1000;
    uint32_t _lastBlynkUpdate = 0;

    void processPendingEvents();
    void runStateMachine();

    bool startManualIrrigation(uint8_t zone);
    bool stopIrrigation();

    void updateBlynk();
};

