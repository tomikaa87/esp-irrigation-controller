#include "IrrigationController.h"

#include "drivers/SimpleI2C.h"

IrrigationController::IrrigationController()
    : _ntpClient(_systemClock)
    , _blynk(Config::BlynkAppToken)
    , _waterTank(_settings)
    , _pumpController(_outputController)
    , _zoneController(_outputController)
    , _otaUpdater("http://tomikaa.noip.me:8001/esp-irrigation-controller/update", _systemClock)
{
    static_assert(Config::Pumps == 1, "Only 1 pump is supported right now");
    static_assert(Config::Zones <= 6, "Only 6 zones supported");

    Drivers::I2C::init();

    _settings.load();
    _zoneController.closeAll();

    _webServer.setZoneStartedHandler([this](const uint8_t zone) {
        return startManualIrrigation(zone);
    });

    _webServer.setStopHandler([this] {
        return stopIrrigation();
    });
}

void IrrigationController::task()
{
    _systemClock.task();
    _flowSensor.task();
    _webServer.task();
    _ntpClient.task();
    _otaUpdater.task();
    _blynk.task();

    if (millis() - _lastTaskCallMillis >= 100) {
        _lastTaskCallMillis = millis();

        if (_taskCallCount++ % 10 == 0)
        {
            _scheduler.task();
            processPendingEvents();
        }

        runStateMachine();
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
    }
}

void ICACHE_RAM_ATTR IrrigationController::epochTimerIsr()
{
    _systemClock.timerIsr();
}

void IrrigationController::processPendingEvents()
{
    if (_state != State::Idle || !_scheduler.hasPendingEvents() || _pumpController.isRunning(0))
        return;

    const auto e = _scheduler.nextPendingEvent();

    std::cout << "starting irrigation, amount=" << e.amount << ", zone=" << static_cast<int>(e.zone) << std::endl;

    _requiredAmount = e.amount;
    _activeZone = e.zone;
    _state = State::Starting;
}

void IrrigationController::runStateMachine()
{
    switch (_state)
    {
        case State::Idle:
            break;

        case State::Error:
            break;

        case State::Starting:
            _zoneController.open(_activeZone);
            _flowSensor.reset();
            _pumpController.start(0);
            _flowErrorCount = 0;
            _state = State::Pumping;
            break;

        case State::Pumping:
        {
            if (_lastFlowSensorTicks + _settings.data.flowSensor.errorDetectionTicks >= _flowSensor.ticks())
            {
                ++_flowErrorCount;

                std::cout << "insufficient water flow detected, errorCount=" << static_cast<int>(_flowErrorCount) << std::endl;
                std::cout << "lastFlowSensorTicks=" << _lastFlowSensorTicks << std::endl;

                if (_flowErrorCount > 3)
                {
                    std::cout << "error: insufficient water flow detected, aborting" << std::endl;
                    _pumpController.stop(0);
                    _zoneController.closeAll();
                    const Decilitres pumpedAmount = _flowSensor.ticks() / _settings.data.flowSensor.ticksPerDecilitres;
                    _waterTank.use(pumpedAmount);
                    _state = State::Error;
                    break;
                }
            }

            _lastFlowSensorTicks = _flowSensor.ticks();

            const Decilitres pumpedAmount = _flowSensor.ticks() / _settings.data.flowSensor.ticksPerDecilitres;
            std::cout << "pumping, pumpedAmount=" << pumpedAmount << std::endl;
            if (!_manualIrrigation && pumpedAmount >= _requiredAmount)
            {
                std::cout << "the required amount has been pumped out" << std::endl;
                _state = State::Stopping;
            }
            break;
        }

        case State::Stopping:
        {
            std::cout << "stopping the irrigation cycle" << std::endl;
            _pumpController.stop(0);
            _zoneController.closeAll();
            const Decilitres pumpedAmount = _flowSensor.ticks() / _settings.data.flowSensor.ticksPerDecilitres;
            _waterTank.use(pumpedAmount);
            _state = State::Idle;
            _manualIrrigation = false;
            break;
        }
    }
}


bool IrrigationController::startManualIrrigation(const uint8_t zone)
{
    if (_state != State::Idle) {
        _log.warning("cannot start irrigation, pump is active");
        return false;
    }

    _log.info("starting manual irrigation: zone=%u", zone);

    _manualIrrigation = true;
    _activeZone = zone;
    _state = State::Starting;

    return true;
}

bool IrrigationController::stopIrrigation()
{
    if (_state != State::Pumping) {
        _log.warning("cannot stop irrigation, pump is already idle");
        return false;
    }

    _log.info("stopping irrigation");

    _state = State::Stopping;

    return true;
}

void IrrigationController::updateBlynk()
{

}