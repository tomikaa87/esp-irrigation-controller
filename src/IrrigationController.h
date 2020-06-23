#pragma once

#include "FlowSensor.h"
#include "LedController.h"
#include "Logger.h"
#include "PumpController.h"
#include "Scheduler.h"
#include "Settings.h"
#include "WaterTank.h"
#include "WebServer.h"
#include "ZoneController.h"

#include <Adafruit_MCP23008.h>

class IrrigationController
{
public:
    IrrigationController();

    void task();

private:
    Logger _log{ "IrrigationController" };
    Adafruit_MCP23008 _ioExpander;
    Settings _settings;
    FlowSensor _flowSensor;
    PumpController _pumpController;
    Scheduler _scheduler;
    ZoneController _zoneController;
    WaterTank _waterTank;
    LedController _ledController;
    WebServer _webServer;

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

    void processPendingEvents();
    void runStateMachine();

    bool startManualIrrigation(uint8_t zone);
    bool stopIrrigation();
};

