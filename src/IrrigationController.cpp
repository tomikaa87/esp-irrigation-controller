#include "IrrigationController.h"

#include <algorithm>
#include <array>

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

        _log.debug("executing next task: pump=%d, zone=%u, amount=%u dl", unit.pump.id(), task.zone, task.amount);

        if (!unit.pump.start(task.zone, task.amount)) {
            _log.error("can't start pumping");
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

    _log.debug("loading scheduler event zone=%u, amount=%u", event.amount, event.zone);

    enqueueTask(event.amount, event.zone);
}

bool IrrigationController::enqueueTask(const uint8_t zone, const Decilitres amount, const bool manual)
{
    _log.debug("creating pump task: zone=%u, amount=%u, manual=%s",
        zone, amount, manual ? "yes" : "no"
    );

    if (_draining) {
        _log.warning("can't add task during draining");
        return false;
    }

    auto pumpForZone = std::find_if(
        _pumpUnits.begin(),
        _pumpUnits.end(),
        [zone](const PumpUnit& unit) { return unit.pump.containsZone(zone); }
    );

    if (pumpForZone == _pumpUnits.end()) {
        _log.error("can't find pump for zone %u", zone);
        return false;
    }

    pumpForZone->taskQueue.emplace_back(PumpUnit::Task{ zone, amount, manual });

    return true;
}

bool IrrigationController::removeTasksForZone(const uint8_t zone)
{
    _log.debug("removing pump tasks: zone=%u", zone);

    auto pumpForZone = std::find_if(
        _pumpUnits.begin(),
        _pumpUnits.end(),
        [zone](const PumpUnit& unit) { return unit.pump.containsZone(zone); }
    );

    if (pumpForZone == _pumpUnits.end()) {
        _log.error("can't find pump for zone %u", zone);
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
        _log.debug("tasks removed from the queue: %d", originalSize - newSize);
    }

    if (pumpForZone->pump.activeZone() == zone && pumpForZone->pump.isRunning()) {
        _log.debug("stopping the irrigation in the specified zone: zone=%u", zone);

        pumpForZone->pump.stop();
    }

    return true;
}

bool IrrigationController::enqueueTaskWithStoredAmount(const uint8_t zone)
{
    if (zone >= Config::Zones) {
        _log.error("can't add task for invalid zone: %u", zone);
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
    _log.info("start draining process");

    _draining = true;
    _zoneController.open(zone);
}

void IrrigationController::stopDraining()
{
    _log.info("stop draining process");

    _draining = false;
    _zoneController.closeAll();
}

void IrrigationController::setupWebServer()
{
    _log.info("setting up web server");

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

    _log.info("setting up the MQTT interface");

    for (unsigned zone = 0; zone < Config::Zones; ++zone) {
        _mqtt.zonePresetAmounts[zone].setChangedHandler([this, zone](const int newAmount) {
            _log.debug("MQTT: zone preset amount changed: zone=%u, amount=%d", zone, newAmount);
            _settings.data.irrigation.amounts[zone] = newAmount;
            _settings.save();
        });

        _mqtt.zoneActiveStates[zone].setChangedHandler([this, zone](const bool activate) {
            _log.debug("MQTT: zone active state changed: zone=%u, activate=%u", zone, activate);
            if (activate) {
                enqueueTaskWithStoredAmount(zone);
            } else {
                removeTasksForZone(zone);
            }
        });
    }

    _zoneController.addZoneChangedHandler([this](const uint8_t zone, const bool open) {
        _log.debug("MQTT: updating zone in-use state: zone=%u, open=%u", zone, open);
        if (zone < _mqtt.zoneInUseStates.size()) {
            _mqtt.zoneInUseStates[zone] = open;
        }
    });

    for (unsigned pumpUnit = 0; pumpUnit < _pumpUnits.size(); ++pumpUnit) {
        _pumpUnits[pumpUnit].pump.addStateChangedHandler([this, pumpUnit](const bool running) {
            _log.debug("MQTT: updating pump active state: pumpUnit=%u, running=%u", pumpUnit, running);
            _mqtt.pumpActiveStates[pumpUnit] = running;
        });
    }
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
}