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
#include <network/MQTT/MqttVariable.h>

#include <queue>

class MqttClient;

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

    struct Mqtt {
        Mqtt(
            MqttClient& mqttClient
        )
            : zone1Active(PSTR("irrigctl/zone/1/active"), PSTR("irrigctl/zone/1/active/set"), mqttClient)
            , zone2Active(PSTR("irrigctl/zone/2/active"), PSTR("irrigctl/zone/2/active/set"), mqttClient)
            , zone3Active(PSTR("irrigctl/zone/3/active"), PSTR("irrigctl/zone/3/active/set"), mqttClient)
            , zone4Active(PSTR("irrigctl/zone/4/active"), PSTR("irrigctl/zone/4/active/set"), mqttClient)
            , zone5Active(PSTR("irrigctl/zone/5/active"), PSTR("irrigctl/zone/5/active/set"), mqttClient)
            , zone6Active(PSTR("irrigctl/zone/6/active"), PSTR("irrigctl/zone/6/active/set"), mqttClient)
            , zone1PumpedAmount(PSTR("irrigctl/zone/1/pumpedAmount"), mqttClient)
            , zone2PumpedAmount(PSTR("irrigctl/zone/2/pumpedAmount"), mqttClient)
            , zone3PumpedAmount(PSTR("irrigctl/zone/3/pumpedAmount"), mqttClient)
            , zone4PumpedAmount(PSTR("irrigctl/zone/4/pumpedAmount"), mqttClient)
            , zone5PumpedAmount(PSTR("irrigctl/zone/5/pumpedAmount"), mqttClient)
            , zone6PumpedAmount(PSTR("irrigctl/zone/6/pumpedAmount"), mqttClient)
            , zone1PresetAmount(PSTR("irrigctl/zone/1/presetAmount"), PSTR("irrigctl/zone/1/presetAmount/set"), mqttClient)
            , zone2PresetAmount(PSTR("irrigctl/zone/2/presetAmount"), PSTR("irrigctl/zone/2/presetAmount/set"), mqttClient)
            , zone3PresetAmount(PSTR("irrigctl/zone/3/presetAmount"), PSTR("irrigctl/zone/3/presetAmount/set"), mqttClient)
            , zone4PresetAmount(PSTR("irrigctl/zone/4/presetAmount"), PSTR("irrigctl/zone/4/presetAmount/set"), mqttClient)
            , zone5PresetAmount(PSTR("irrigctl/zone/5/presetAmount"), PSTR("irrigctl/zone/5/presetAmount/set"), mqttClient)
            , zone6PresetAmount(PSTR("irrigctl/zone/6/presetAmount"), PSTR("irrigctl/zone/6/presetAmount/set"), mqttClient)
            , pump1Active(PSTR("irrigctl/pump/1/active"), mqttClient)
            , pump2Active(PSTR("irrigctl/pump/2/active"), mqttClient)
        {}

        MqttVariable<bool> zone1Active;
        MqttVariable<bool> zone2Active;
        MqttVariable<bool> zone3Active;
        MqttVariable<bool> zone4Active;
        MqttVariable<bool> zone5Active;
        MqttVariable<bool> zone6Active;

        MqttVariable<int> zone1PumpedAmount;
        MqttVariable<int> zone2PumpedAmount;
        MqttVariable<int> zone3PumpedAmount;
        MqttVariable<int> zone4PumpedAmount;
        MqttVariable<int> zone5PumpedAmount;
        MqttVariable<int> zone6PumpedAmount;

        MqttVariable<Decilitres> zone1PresetAmount;
        MqttVariable<Decilitres> zone2PresetAmount;
        MqttVariable<Decilitres> zone3PresetAmount;
        MqttVariable<Decilitres> zone4PresetAmount;
        MqttVariable<Decilitres> zone5PresetAmount;
        MqttVariable<Decilitres> zone6PresetAmount;

        MqttVariable<bool> pump1Active;
        MqttVariable<bool> pump2Active;
    } _mqtt;

    void processTasks();
    void processPendingEvents();

    bool enqueueTask(uint8_t zone, Decilitres amount, bool manual = false);
    bool enqueueTaskWithStoredAmount(uint8_t zone);

    bool startManualIrrigation(uint8_t zone);
    void stopIrrigation();

    void setupBlynk();

    void updateBlynk();
    void updateBlynkStatus();

    void startDraining(uint8_t zone);
    void stopDraining();

    void setupWebServer();

    void setupMqtt();
};

