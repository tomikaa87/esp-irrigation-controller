#include "IrrigationController.h"

#include <algorithm>
#include <array>

IrrigationController::IrrigationController(const ApplicationConfig& appConfig)
    : _appConfig(appConfig)
    , _coreApplication(_appConfig)
    , _settings(_coreApplication.settings())
    , _blynk(_coreApplication.blynkHandler(), _settings)
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
    setupBlynk();
    setupMqtt();
}

void IrrigationController::task()
{
    _flowSensor.task();
    _webServer.task();
    _coreApplication.task();
    _blynk.task();

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
    bool idle = true;

    for (auto& unit : _pumpUnits) {
        if (!unit.taskQueue.empty() || unit.pump.isRunning()) {
            idle = false;
        }

        if (unit.taskQueue.empty() || unit.pump.isRunning()) {
            continue;
        }

        const auto task = unit.taskQueue.front();
        unit.taskQueue.pop();

        _log.debug("executing next task: pump=%d, zone=%u, amount=%u dl", unit.pump.id(), task.zone, task.amount);

        if (!unit.pump.start(task.zone, task.amount)) {
            _log.error("can't start pumping");
        }
    }

    if (_irrigationStartedFromBlynk && idle) {
        _irrigationStartedFromBlynk = false;
        _blynk.setControlsEnabled(true);
        _blynk.resetSelectors();
    }
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

    _log.debug("creating pump task: zone=%u, amount=%u, manual=%s",
        zone, amount, manual ? "yes" : "no"
    );

    pumpForZone->taskQueue.emplace(PumpUnit::Task{ zone, amount, manual });

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

    _mqtt.zone1PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[0] = v;
        _settings.save();
    });

    _mqtt.zone2PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[1] = v;
        _settings.save();
    });

    _mqtt.zone3PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[2] = v;
        _settings.save();
    });

    _mqtt.zone4PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[3] = v;
        _settings.save();
    });

    _mqtt.zone5PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[4] = v;
        _settings.save();
    });

    _mqtt.zone6PresetAmount.setChangedHandler([this](const int v) {
        _settings.data.irrigation.amounts[5] = v;
        _settings.save();
    });

    _zoneController.addZoneChangedHandler([this](const uint8_t zone, const bool open) {
        const std::array<MqttVariable<bool>*, Config::Zones> zoneActiveStates{
            &_mqtt.zone1Active,
            &_mqtt.zone2Active,
            &_mqtt.zone3Active,
            &_mqtt.zone4Active,
            &_mqtt.zone5Active,
            &_mqtt.zone6Active,
        };

        if (zone < zoneActiveStates.size()) {
            *zoneActiveStates[zone] = open;
        }
    });

    _pumpUnits[0].pump.addStateChangedHandler([this](const bool running) {
        _mqtt.pump1Active = running;
    });
}

void IrrigationController::updateMqtt()
{
    _mqtt.zone1PresetAmount = _settings.data.irrigation.amounts[0];
    _mqtt.zone2PresetAmount = _settings.data.irrigation.amounts[1];
    _mqtt.zone3PresetAmount = _settings.data.irrigation.amounts[2];
    _mqtt.zone4PresetAmount = _settings.data.irrigation.amounts[3];
    _mqtt.zone5PresetAmount = _settings.data.irrigation.amounts[4];
    _mqtt.zone6PresetAmount = _settings.data.irrigation.amounts[5];
}