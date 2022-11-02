#include "IrrigationController.h"
#include "Utils.h"

#include <algorithm>
#include <array>
#include <sstream>

IrrigationController::IrrigationController(const ApplicationConfig& appConfig)
    : _appConfig(appConfig)
    , _coreApplication(_appConfig)
    , _settings(_coreApplication.settings())
#ifdef IOT_ENABLE_BLYNK
    , _blynk(_coreApplication.blynkHandler(), _settings)
#endif
    , _waterTank(_settings)
    , _zoneController(_outputController)
    , _scheduler(_coreApplication.systemClock())
    , _webServer(_appConfig, _flowSensor)
    , _pumpUnits{
        PumpUnit{ Pump{ 0, { 0, 1, 2, 3, 4, 5 }, _flowSensor, _outputController, _zoneController, _settings } }
    }
    , _mqtt(_coreApplication.mqttClient())
{
    static_assert(Config::Zones <= 6, "Only 6 zones supported");

    _settings.load();
    _zoneController.closeAll();

    for (auto i = 0u; i < _pumpUnits.size(); ++i) {
        _pumpUnits[i].pump.setErrorHandler([this, i](const Pump::Error error) {
            onPumpError(i, error);
        });
    }

    setupWebServer();
#ifdef IOT_ENABLE_BLYNK
    setupBlynk();
#endif
    setupMqtt();
}

void IrrigationController::task()
{
    _flowSensor.task();
    _webServer.task();
    _coreApplication.task();
#ifdef IOT_ENABLE_BLYNK
    _blynk.task();
#endif

    for (auto& unit : _pumpUnits) {
        unit.pump.task();
    }

    processTasks();

    if (millis() - _lastTaskCallMillis >= 100) {
        _lastTaskCallMillis = millis();

        if (_taskCallCount++ % 10 == 0)
        {
            _scheduler.task();
            processPendingEvents();
        }
    }
}

void IrrigationController::processTasks()
{
    [[maybe_unused]] bool idle = true;

    for (auto& unit : _pumpUnits) {
        if (!unit.taskQueue.empty() || unit.pump.isRunning()) {
            idle = false;
        }

        if (unit.taskQueue.empty() || unit.pump.isRunning()) {
            continue;
        }

        const auto task = unit.taskQueue.front();
        unit.taskQueue.pop_front();

        _log.debug_P(PSTR("executing next task: pump=%d, zone=%u, amount=%u dl"), unit.pump.id(), task.zone, task.amount);

        if (!unit.pump.start(task.zone, task.amount)) {
            _log.error_P(PSTR("can't start pumping"));
        }
    }

#ifdef IOT_ENABLE_BLYNK
    if (_irrigationStartedFromBlynk && idle) {
        _irrigationStartedFromBlynk = false;
        _blynk.setControlsEnabled(true);
        _blynk.resetSelectors();
    }
#endif
}

void IrrigationController::processPendingEvents()
{
    if (!_scheduler.hasPendingEvents()) {
        return;
    }

    const auto event = _scheduler.nextPendingEvent();

    _log.debug_P(PSTR("loading scheduler event zone=%u, amount=%u"), event.amount, event.zone);

    enqueueTask(event.amount, event.zone);
}

bool IrrigationController::enqueueTask(const uint8_t zone, const Decilitres amount, const bool manual)
{
    _log.debug("creating pump task: zone=%u, amount=%u, manual=%s",
        zone, amount, manual ? "yes" : "no"
    );

    if (_draining) {
        _log.warning_P(PSTR("can't add task during draining"));
        return false;
    }

    auto pumpForZone = std::find_if(
        _pumpUnits.begin(),
        _pumpUnits.end(),
        [zone](const PumpUnit& unit) { return unit.pump.containsZone(zone); }
    );

    if (pumpForZone == _pumpUnits.end()) {
        _log.error_P(PSTR("can't find pump for zone %u"), zone);
        return false;
    }

    pumpForZone->taskQueue.emplace_back(PumpUnit::Task{ zone, amount, manual });

    return true;
}

bool IrrigationController::removeTasksForZone(const uint8_t zone)
{
    _log.debug_P(PSTR("removing pump tasks: zone=%u"), zone);

    auto pumpForZone = std::find_if(
        _pumpUnits.begin(),
        _pumpUnits.end(),
        [zone](const PumpUnit& unit) { return unit.pump.containsZone(zone); }
    );

    if (pumpForZone == _pumpUnits.end()) {
        _log.error_P(PSTR("can't find pump for zone %u"), zone);
        return false;
    }

    const auto originalSize = pumpForZone->taskQueue.size();

    pumpForZone->taskQueue.erase(
        std::remove_if(
            std::begin(pumpForZone->taskQueue),
            std::end(pumpForZone->taskQueue),
            [zone](const PumpUnit::Task& t) {
                return t.zone == zone;
            }
        ),
        std::end(pumpForZone->taskQueue)
    );

    const auto newSize = pumpForZone->taskQueue.size();

    if (newSize < originalSize) {
        _log.debug_P(PSTR("tasks removed from the queue: %d"), originalSize - newSize);
    }

    if (pumpForZone->pump.activeZone() == zone && pumpForZone->pump.isRunning()) {
        _log.debug_P(PSTR("stopping the irrigation in the specified zone: zone=%u"), zone);

        pumpForZone->pump.stop();
    }

    return true;
}

bool IrrigationController::enqueueTaskWithStoredAmount(const uint8_t zone)
{
    if (zone >= Config::Zones) {
        _log.error_P(PSTR("can't add task for invalid zone: %u"), zone);
        return false;
    }

    const auto amount = _settings.data.irrigation.amounts[zone];

    return enqueueTask(zone, amount);
}

bool IrrigationController::startManualIrrigation(const uint8_t zone)
{
    return enqueueTask(zone, 0, true);
}

void IrrigationController::stopIrrigation()
{
    for (auto& unit : _pumpUnits) {
        unit.pump.stop();
    }
}

#ifdef IOT_ENABLE_BLYNK
void IrrigationController::setupBlynk()
{
    _blynk.setStartHandler([this](const std::vector<Blynk::StartedZone>& zones) {
        _blynk.setControlsEnabled(false);
        _irrigationStartedFromBlynk = true;

        for (const auto& z : zones) {
            enqueueTask(z.zone, z.amount);
        }
    });

    _blynk.setStopHandler([this] {
        stopIrrigation();
        _blynk.setControlsEnabled(true);
        _irrigationStartedFromBlynk = false;
    });

    _coreApplication.setBlynkUpdateHandler([this] {
        updateBlynk();
    });
}

void IrrigationController::updateBlynk()
{
    _blynk.setFlowSensorTicks(_flowSensor.ticks());
    updateBlynkStatus();
}

void IrrigationController::updateBlynkStatus()
{
    std::string s;
    s.reserve(30);

    bool hasRunningPump = false;
    for (const auto& pu : _pumpUnits) {
        if (!pu.pump.isRunning()) {
            continue;
        }

        hasRunningPump = true;

        if (!s.empty()) {
            s += ", ";
        } else {
            s += "Pumping: ";
        }

        s += "zone " + std::to_string(pu.pump.activeZone() + 1);
        if (pu.pump.isManual()) {
            s += " (manual)";
        } else {
            s += " (" + std::to_string(pu.pump.remainingAmount()) + " dl)";
        }
    }

    if (!hasRunningPump) {
        s = "Idle";
    }

    _blynk.setStatusText(s);
}
#endif

void IrrigationController::startDraining(const uint8_t zone)
{
    _log.info_P(PSTR("start draining process"));

    _draining = true;
    _zoneController.open(zone);
}

void IrrigationController::stopDraining()
{
    _log.info_P(PSTR("stop draining process"));

    _draining = false;
    _zoneController.closeAll();
}

void IrrigationController::setupWebServer()
{
    _log.info_P(PSTR("setting up web server"));

    _webServer.setZoneStartedHandler([this](const uint8_t zone) {
        return startManualIrrigation(zone);
    });

    _webServer.setStopHandler([this] {
        stopIrrigation();
        return true;
    });

    _webServer.setStartDrainingHandler([this](const uint8_t zone) {
        startDraining(zone);
    });

    _webServer.setStopDrainingHandler([this] { stopDraining(); });

    _webServer.setEnqueueStoredHandler([this](const uint8_t zone) {
        enqueueTaskWithStoredAmount(zone);
    });
}

void IrrigationController::setupMqtt()
{
    _coreApplication.setMqttUpdateHandler([this] {
        updateMqtt();
    });

    _log.info_P(PSTR("setting up the MQTT interface"));

    for (unsigned zone = 0; zone < Config::Zones; ++zone) {
        _mqtt.zonePresetAmounts[zone].setChangedHandler([this, zone](const int newAmount) {
            _log.debug_P(PSTR("MQTT: zone preset amount changed: zone=%u, amount=%d"), zone, newAmount);
            _settings.data.irrigation.amounts[zone] = newAmount;
            _settings.save();
        });

        _mqtt.zoneActiveStates[zone].setChangedHandler([this, zone](const bool activate) {
            _log.debug_P(PSTR("MQTT: zone active state changed: zone=%u, activate=%u"), zone, activate);
            if (activate) {
                enqueueTaskWithStoredAmount(zone);
            } else {
                removeTasksForZone(zone);
            }
        });
    }

    _zoneController.addZoneChangedHandler([this](const uint8_t zone, const bool open) {
        _log.debug_P(PSTR("MQTT: updating zone in-use state: zone=%u, open=%u"), zone, open);
        if (zone < _mqtt.zoneInUseStates.size()) {
            _mqtt.zoneInUseStates[zone] = open;
        }
    });

    for (unsigned pumpUnit = 0; pumpUnit < _pumpUnits.size(); ++pumpUnit) {
        _pumpUnits[pumpUnit].pump.addStateChangedHandler([this, pumpUnit](const bool running) {
            _log.debug_P(PSTR("MQTT: updating pump active state: pumpUnit=%u, running=%u"), pumpUnit, running);
            _mqtt.pumpActiveStates[pumpUnit] = running;
        });
    }

    const auto makeValveConfig = [](const int zone) {
        std::stringstream config;

        config << '{';
        config << Utils::pgmToStdString(PSTR(R"("icon":"mdi:pipe-valve")"));
        config << Utils::pgmToStdString(PSTR(R"(,"name":"Irrigation Zone )")) << std::to_string(zone) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"object_id":"irrigctl_zone_)")) << std::to_string(zone) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"unique_id":"irrigctl_zone_)")) << std::to_string(zone) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"command_topic":"irrigctl/zone/)")) << std::to_string(zone) << Utils::pgmToStdString(PSTR("/active/set")) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"state_topic":"irrigctl/zone/)")) << std::to_string(zone) << Utils::pgmToStdString(PSTR("/active")) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"payload_on":"1")"));
        config << Utils::pgmToStdString(PSTR(R"(,"payload_off":"0")"));
        config << Utils::pgmToStdString(PSTR(R"(,"state_on":"1")"));
        config << Utils::pgmToStdString(PSTR(R"(,"state_off":"0")"));
        config << '}';

        return config.str();
    };

    const auto makeAmountSettingConfig = [](const int zone) {
        std::stringstream config;

        config << '{';
        config << Utils::pgmToStdString(PSTR(R"("icon":"mdi:water")"));
        config << Utils::pgmToStdString(PSTR(R"(,"name":"Irrigation Zone )")) << std::to_string(zone) << Utils::pgmToStdString(PSTR(" Amount")) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"object_id":"irrigctl_zone_amount_)")) << std::to_string(zone) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"unique_id":"irrigctl_zone_amount_)")) << std::to_string(zone) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"command_topic":"irrigctl/zone/)")) << std::to_string(zone) << Utils::pgmToStdString(PSTR("/presetAmount/set")) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"state_topic":"irrigctl/zone/)")) << std::to_string(zone) << Utils::pgmToStdString(PSTR("/presetAmount")) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"min":0)"));
        config << Utils::pgmToStdString(PSTR(R"(,"max":50)"));
        config << Utils::pgmToStdString(PSTR(R"(,"step":1)"));
        config << Utils::pgmToStdString(PSTR(R"(,"unit_of_measurement":"dL")"));
        config << '}';

        return config.str();
    };

    const auto makeSensorConfig = [](
        PGM_P name,
        PGM_P id,
        PGM_P stateTopic,
        const bool forAmounts = true
    ) {
        std::stringstream config;

        config << '{';
        if (forAmounts) {
            config << Utils::pgmToStdString(PSTR(R"("icon":"mdi:water")"));
        } else {
            config << Utils::pgmToStdString(PSTR(R"("icon":"mdi:pipe-valve")"));
        }
        config << Utils::pgmToStdString(PSTR(R"(,"name":")")) << Utils::pgmToStdString(name) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"object_id":")")) << Utils::pgmToStdString(id) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"unique_id":")")) << Utils::pgmToStdString(id) << '"';
        config << Utils::pgmToStdString(PSTR(R"(,"state_topic":")")) << Utils::pgmToStdString(stateTopic) << '"';
        if (forAmounts) {
            config << Utils::pgmToStdString(PSTR(R"(,"unit_of_measurement":"dL")"));
        }
        config << '}';

        return config.str();
    };

    for (auto i = 1u; i <= Config::Zones; ++i) {
        const auto valveConfigTopic =
            Utils::pgmToStdString(PSTR("homeassistant/switch/irrigctl_zone_"))
            + std::to_string(i)
            + Utils::pgmToStdString(PSTR("/config"));

        const auto amountSettingConfigTopic =
            Utils::pgmToStdString(PSTR("homeassistant/number/irrigctl_zone_amount_"))
            + std::to_string(i)
            + Utils::pgmToStdString(PSTR("/config"));

        _coreApplication.mqttClient().publish(valveConfigTopic, makeValveConfig(i), false);
        _coreApplication.mqttClient().publish(amountSettingConfigTopic, makeAmountSettingConfig(i), false);
    }

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_active_zone_preset_amount/config"),
        makeSensorConfig(
            PSTR("Active Irrigation Zone Preset Amount"),
            PSTR("irrigctl_active_zone_preset_amount"),
            PSTR("irrigctl/zone/active/presetAmount")
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_active_zone_pumped_amount/config"),
        makeSensorConfig(
            PSTR("Active Irrigation Zone Pumped Amount"),
            PSTR("irrigctl_active_zone_pumped_amount"),
            PSTR("irrigctl/zone/active/pumpedAmount")
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_active_zone/config"),
        makeSensorConfig(
            PSTR("Active Irrigation Zone"),
            PSTR("irrigctl_active_zone"),
            PSTR("irrigctl/zone/active"),
            false
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_zone_error_count/config"),
        makeSensorConfig(
            PSTR("Zone Error Count"),
            PSTR("irrigctl_zone_error_count"),
            PSTR("irrigctl/zone/errorCount"),
            false
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_last_errored_zone/config"),
        makeSensorConfig(
            PSTR("Last Errored Zone"),
            PSTR("irrigctl_last_errored_zone"),
            PSTR("irrigctl/zone/lastErrored"),
            false
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_last_active_zone/config"),
        makeSensorConfig(
            PSTR("Last Active Zone"),
            PSTR("irrigctl_last_active_zone"),
            PSTR("irrigctl/zone/lastActive"),
            false
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_last_errored_pump/config"),
        makeSensorConfig(
            PSTR("Last Errored Pump"),
            PSTR("irrigctl_last_errored_pump"),
            PSTR("irrigctl/pump/lastErrored"),
            false
        ),
        false
    );

    _coreApplication.mqttClient().publish(
        PSTR("homeassistant/sensor/irrigctl_last_pump_error/config"),
        makeSensorConfig(
            PSTR("Last Pump Error"),
            PSTR("irrigctl_last_pump_error"),
            PSTR("irrigctl/pump/lastError"),
            false
        ),
        false
    );

    _mqtt.activeZone = 0;
    _mqtt.activeZonePresetAmount = 0;
    _mqtt.activeZonePumpedAmount = 0;
    _mqtt.zoneErrorCount = 0;
    _mqtt.lastErroredZone = -1;
    _mqtt.lastActiveZone = -1;
    _mqtt.lastErroredPump = -1;
}

void IrrigationController::updateMqtt()
{
    for (unsigned zone = 0; zone < Config::Zones; ++zone) {
        _mqtt.zonePresetAmounts[zone] = _settings.data.irrigation.amounts[zone];

        if (_mqtt.zoneInUseStates[zone]) {
            _mqtt.zoneActiveStates[zone] = true;
            continue;
        }

        _mqtt.zoneActiveStates[zone] = std::any_of(
            std::begin(_pumpUnits[0].taskQueue),
            std::end(_pumpUnits[0].taskQueue),
            [zone](const PumpUnit::Task& t) {
                return t.zone == zone;
            }
        );
    }

    if (_pumpUnits[0].pump.isRunning()) {
        const auto activeZone = _pumpUnits[0].pump.activeZone();
        _mqtt.activeZone = activeZone + 1; // Indexed from 1
        _mqtt.lastActiveZone = static_cast<int>(_mqtt.activeZone);
        _mqtt.activeZonePresetAmount = _settings.data.irrigation.amounts[activeZone];
        _mqtt.activeZonePumpedAmount = _pumpUnits[0].pump.pumpedAmount();
    } else {
        _mqtt.activeZone = 0;
        _mqtt.activeZonePresetAmount = 0;
        _mqtt.activeZonePumpedAmount = 0;
    }
}

void IrrigationController::onPumpError(const int pump, const Pump::Error error)
{
    _mqtt.zoneErrorCount = static_cast<int>(_mqtt.zoneErrorCount) + 1;

    _mqtt.lastErroredPump = pump + 1;

    _mqtt.lastErroredZone = static_cast<int>(_mqtt.activeZone);

    _mqtt.lastPumpError = [&]() -> std::string {
        switch (error) {
            case Pump::Error::LeakDetected:
                return "LeakDetected";
            case Pump::Error::FlowRateTooLow:
                return "FlowRateTooLow";
            case Pump::Error::Busy:
                return "Busy";
        }
        return "Unknown";
    }();

    _log.error_P(
        PSTR("onPumpError: pump=%d, zone=%d, error=%s"),
        static_cast<int>(_mqtt.lastErroredPump),
        static_cast<int>(_mqtt.lastErroredZone),
        static_cast<std::string>(_mqtt.lastPumpError).c_str()
    );
}