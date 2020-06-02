#pragma once

#include "FlowSensor.h"
#include "PumpController.h"
#include "Scheduler.h"
#include "Settings.h"
#include "WaterTank.h"
#include "ZoneController.h"

class IrrigationController
{
public:
    IrrigationController();

    void task();

private:
    Settings _settings;
    FlowSensor _flowSensor;
    PumpController _pumpController;
    Scheduler _scheduler;
    ZoneController _zoneController;
    WaterTank _waterTank;

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

    void processPendingEvents();
    void runStateMachine();
};

