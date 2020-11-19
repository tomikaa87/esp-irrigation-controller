#pragma once

#include "Blynk.h"
#include "FlowSensor.h"
#include "LedController.h"
#include "OutputController.h"
#include "Pump.h"
#include "Scheduler.h"
#include "Settings.h"
#include "WaterTank.h"
#include "WebServer.h"
#include "ZoneController.h"

#include <CoreApplication.h>
#include <Logger.h>

#include <queue>

class IrrigationController
{
public:
    IrrigationController(const ApplicationConfig& appConfig);

    void task();

private:
    Logger _log{ "IrrigationController" };
    const ApplicationConfig& _appConfig;
    CoreApplication _coreApplication;
    Settings _settings;

    Blynk _blynk;
    OutputController _outputController;
    FlowSensor _flowSensor;
    WaterTank _waterTank;
    ZoneController _zoneController;
    Scheduler _scheduler;
    LedController _ledController;
    WebServer _webServer;

    struct PumpUnit {
        struct Task {
            uint8_t zone = 0;
            Decilitres amount = 0;
            bool manual = false;

            Task(const uint8_t zone, const Decilitres amount, const bool manual = false)
                : zone(zone)
                , amount(amount)
                , manual(manual)
            {}
        };

        explicit PumpUnit(Pump pump)
            : pump(std::move(pump))
        {}

        Pump pump;
        std::queue<Task> taskQueue;
    };

    std::vector<PumpUnit> _pumpUnits;

    int32_t _lastTaskCallMillis = 0;
    std::size_t _taskCallCount = 0;

    bool _updateChecked = false;
    uint32_t _updateCheckTimer = 0;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t _lastSlowLoopUpdate = 0;

    bool _irrigationStartedFromBlynk = false;
    bool _draining = false;

    void processTasks();
    void processPendingEvents();

    bool enqueueTask(uint8_t zone, Decilitres amount, bool manual = false);

    bool startManualIrrigation(uint8_t zone);
    void stopIrrigation();

    void setupBlynk();

    void updateBlynk();
    void updateBlynkStatus();

    void startDraining(uint8_t zone);
    void stopDraining();
};

