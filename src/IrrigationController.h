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

#include <array>
#include <deque>

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

#ifdef IOT_ENABLE_BLYNK
    Blynk _blynk;
#endif
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
        std::deque<Task> taskQueue;
    };

    std::vector<PumpUnit> _pumpUnits;

    int32_t _lastTaskCallMillis = 0;
    std::size_t _taskCallCount = 0;

    bool _updateChecked = false;
    uint32_t _updateCheckTimer = 0;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t _lastSlowLoopUpdate = 0;

#ifdef IOT_ENABLE_BLYNK
    bool _irrigationStartedFromBlynk = false;
#endif
    bool _draining = false;

    struct Mqtt {
        Mqtt(
            MqttClient& mqttClient
        )
            : zoneActiveStates{
                MqttVariable<bool>{ PSTR("irrigctl/zone/1/active"), PSTR("irrigctl/zone/1/active/set"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/2/active"), PSTR("irrigctl/zone/2/active/set"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/3/active"), PSTR("irrigctl/zone/3/active/set"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/4/active"), PSTR("irrigctl/zone/4/active/set"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/5/active"), PSTR("irrigctl/zone/5/active/set"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/6/active"), PSTR("irrigctl/zone/6/active/set"), mqttClient }
            }
            , zoneInUseStates{
                MqttVariable<bool>{ PSTR("irrigctl/zone/1/inUse"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/2/inUse"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/3/inUse"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/4/inUse"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/5/inUse"), mqttClient },
                MqttVariable<bool>{ PSTR("irrigctl/zone/6/inUse"), mqttClient }
            }
            , zonePumpedAmounts{
                MqttVariable<int>{ PSTR("irrigctl/zone/1/pumpedAmount"), mqttClient },
                MqttVariable<int>{ PSTR("irrigctl/zone/2/pumpedAmount"), mqttClient },
                MqttVariable<int>{ PSTR("irrigctl/zone/3/pumpedAmount"), mqttClient },
                MqttVariable<int>{ PSTR("irrigctl/zone/4/pumpedAmount"), mqttClient },
                MqttVariable<int>{ PSTR("irrigctl/zone/5/pumpedAmount"), mqttClient },
                MqttVariable<int>{ PSTR("irrigctl/zone/6/pumpedAmount"), mqttClient }
            }
            , zonePresetAmounts{
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/1/presetAmount"), PSTR("irrigctl/zone/1/presetAmount/set"), mqttClient },
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/2/presetAmount"), PSTR("irrigctl/zone/2/presetAmount/set"), mqttClient },
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/3/presetAmount"), PSTR("irrigctl/zone/3/presetAmount/set"), mqttClient },
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/4/presetAmount"), PSTR("irrigctl/zone/4/presetAmount/set"), mqttClient },
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/5/presetAmount"), PSTR("irrigctl/zone/5/presetAmount/set"), mqttClient },
                MqttVariable<Decilitres>{ PSTR("irrigctl/zone/6/presetAmount"), PSTR("irrigctl/zone/6/presetAmount/set"), mqttClient }
            }
            , pumpActiveStates{
                MqttVariable<bool>{ PSTR("irrigctl/pump/1/active"), mqttClient }
            }
        {}

        std::array<MqttVariable<bool>, Config::Zones> zoneActiveStates;
        std::array<MqttVariable<bool>, Config::Zones> zoneInUseStates;
        std::array<MqttVariable<int>, Config::Zones> zonePumpedAmounts;
        std::array<MqttVariable<Decilitres>, Config::Zones> zonePresetAmounts;
        std::array<MqttVariable<bool>, Config::Pumps> pumpActiveStates;
    } _mqtt;

    void processTasks();
    void processPendingEvents();

    bool enqueueTask(uint8_t zone, Decilitres amount, bool manual = false);
    bool enqueueTaskWithStoredAmount(uint8_t zone);
    bool removeTasksForZone(uint8_t zone);

    bool startManualIrrigation(uint8_t zone);
    void stopIrrigation();

#ifdef IOT_ENABLE_BLYNK
    void setupBlynk();

    void updateBlynk();
    void updateBlynkStatus();
#endif

    void startDraining(uint8_t zone);
    void stopDraining();

    void setupWebServer();

    void setupMqtt();
    void updateMqtt();
};

