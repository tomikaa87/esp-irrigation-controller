#include "IrrigationController.h"

#include "drivers/SimpleI2C.h"

#include <ArduinoOTA.h>

#include <algorithm>

IrrigationController::IrrigationController()
    : _ntpClient(_systemClock)
    , _blynk(Config::BlynkAppToken, _settings)
    , _waterTank(_settings)
    , _zoneController(_outputController)
    , _scheduler(_systemClock)
    , _webServer(_flowSensor, Config::Network::WebServerPort)
    , _otaUpdater(Config::Network::FirmwareUpdateUrl, _systemClock)
    , _pumpUnits{
        PumpUnit{ Pump{ 0, { 0, 1, 2, 3, 4, 5 }, _flowSensor, _outputController, _zoneController, _settings } }
    }
{
    static_assert(Config::Zones <= 6, "Only 6 zones supported");

    // TODO de-init before SystemClock is destroyed
    Logger::setup(_systemClock);

    Drivers::I2C::init();

    _settings.load();
    _zoneController.closeAll();

    _webServer.setZoneStartedHandler([this](const uint8_t zone) {
        return startManualIrrigation(zone);
    });

    _webServer.setStopHandler([this] {
        stopIrrigation();
        return true;
    });

    setupArduinoOta();
    setupBlynk();
}

void IrrigationController::task()
{
    _systemClock.task();
    _flowSensor.task();
    _webServer.task();
    _ntpClient.task();
    _otaUpdater.task();
    _blynk.task();

    ArduinoOTA.handle();

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

    // Slow loop
    if (_lastSlowLoopUpdate == 0 || millis() - _lastSlowLoopUpdate >= SlowLoopUpdateIntervalMs) {
        _lastSlowLoopUpdate = millis();

        if (!_updateChecked && WiFi.isConnected() && millis() - _updateCheckTimer >= 5000) {
            _updateChecked = true;
            _otaUpdater.forceUpdate();
        }
    }

    // Blynk update loop
    if (_lastBlynkUpdate == 0 || millis() - _lastBlynkUpdate >= BlynkUpdateIntervalMs) {
        _lastBlynkUpdate = millis();
        updateBlynk();
        updateBlynkStatus();
    }
}

void ICACHE_RAM_ATTR IrrigationController::epochTimerIsr()
{
    _systemClock.timerIsr();
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

    if (_startedFromBlynk && idle) {
        _startedFromBlynk = false;
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
    _blynk.setStartHandler([this](const std::vector<BlynkHandler::StartedZone>& zones) {
        _blynk.setControlsEnabled(false);
        _startedFromBlynk = true;

        for (const auto& z : zones) {
            enqueueTask(z.zone, z.amount);
        }
    });

    _blynk.setStopHandler([this] {
        stopIrrigation();
        _blynk.setControlsEnabled(true);
        _startedFromBlynk = false;
    });
}

void IrrigationController::updateBlynk()
{
    _blynk.setFlowSensorTicks(_flowSensor.ticks());
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

void IrrigationController::setupArduinoOta()
{
    ArduinoOTA.onStart([this] {
        _webServer.shutdown();

        auto type = "";
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "flash";
        } else { // U_FS
            type = "file system";
        }

        _log.info("ArduinoOTA: starting update, type=%s", type);
    });

    ArduinoOTA.onEnd([this] {
        _log.info("ArduinoOTA: finished");
    });

    ArduinoOTA.onError([this](const ota_error_t error) {
        auto errorStr = "unknown error";
        switch (error) {
            case OTA_AUTH_ERROR:
                errorStr = "authentication error";
                break;

            case OTA_BEGIN_ERROR:
                errorStr = "begin failed";
                break;

            case OTA_CONNECT_ERROR:
                errorStr = "connect failed";
                break;

            case OTA_END_ERROR:
                errorStr = "end failed";
                break;

            case OTA_RECEIVE_ERROR:
                errorStr = "receive failed";
                break;
        }

        _log.error("ArduinoOTA: update failed, error: %s", errorStr);
    });

    ArduinoOTA.begin();
}