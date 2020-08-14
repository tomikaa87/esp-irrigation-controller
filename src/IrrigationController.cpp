#include "IrrigationController.h"

#include <algorithm>

IrrigationController::IrrigationController()
    : _coreApplication(_appConfig)
    , _settings(_coreApplication.settings())
    , _blynk(_coreApplication.blynkHandler(), _settings)
    , _waterTank(_settings)
    , _zoneController(_outputController)
    , _scheduler(_coreApplication.systemClock())
    , _webServer(_appConfig, _flowSensor, Config::Network::WebServerPort)
    , _pumpUnits{
        PumpUnit{ Pump{ 0, { 0, 1, 2, 3, 4, 5 }, _flowSensor, _outputController, _zoneController, _settings } }
    }
{
    static_assert(Config::Zones <= 6, "Only 6 zones supported");

    _settings.load();
    _zoneController.closeAll();

    _webServer.setZoneStartedHandler([this](const uint8_t zone) {
        return startManualIrrigation(zone);
    });

    _webServer.setStopHandler([this] {
        stopIrrigation();
        return true;
    });

    setupBlynk();
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
