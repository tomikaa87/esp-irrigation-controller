#include "FlowSensor.h"
#include "OutputController.h"
#include "Pump.h"
#include "Settings.h"
#include "ZoneController.h"

#include <string>

Pump::Pump(
    const unsigned id,
    std::set<uint8_t> zones,
    FlowSensor& flowSensor,
    OutputController& outputController,
    ZoneController& zoneController,
    const Settings& settings
)
    : _log(std::string{ "Pump[" } + std::to_string(id) + "]")
    , _id(id)
    , _zones(std::move(zones))
    , _flowSensor(flowSensor)
    , _outputController(outputController)
    , _zoneController(zoneController)
    , _settings(settings)
{}

int Pump::id() const
{
    return _id;
}

void Pump::task()
{
    runStateMachine();
}

bool Pump::start(const uint8_t zone)
{
    return start(zone, 0);
}

bool Pump::start(const uint8_t zone, const Decilitres amount)
{
    const bool manual = amount == 0;

    _log.debug("start reqested: zone=%u, amount=%u, manual=%s",
        zone,
        amount,
        manual ? "yes" : "no"
    );

    if (_state != State::Idle) {
        _log.error_P(PSTR("can't start irrigation, operation is already in progress"));

        if (_errorHandler) {
            _errorHandler(Error::Busy);
        }

        return false;
    }

    _requestedZone = zone;
    _requestedAmount = amount;
    _manualIrrigation = manual;

    changeState(State::StartRequested);

    return true;
}

void Pump::stop()
{
    _log.warning_P(PSTR("manual stop requested"));

    // Stop without leak checking
    stopIrrigation(false);
}

bool Pump::containsZone(const uint8_t zone) const
{
    return _zones.find(zone) != _zones.cend();
}

bool Pump::isRunning() const
{
    return _state != State::Idle;
}

Decilitres Pump::pumpedAmount() const
{
    return _flowSensor.ticks() / Config::FlowSensorTicksPerDecilitre;
}

Decilitres Pump::remainingAmount() const
{
    if (!isRunning() || _manualIrrigation) {
        return 0;
    }

    return std::max(0, _requestedAmount - pumpedAmount());
}

uint8_t Pump::activeZone() const
{
    return _requestedZone;
}

bool Pump::isManual() const
{
    return isRunning() && _manualIrrigation;
}

void Pump::addStateChangedHandler(StateChangedHandler&& handler)
{
    _stateChangedHandlers.emplace_back(std::move(handler));
}

void Pump::setErrorHandler(ErrorHandler&& handler)
{
    _errorHandler = std::move(handler);
}

void Pump::changeState(const State newState)
{
    if (newState == _state) {
        return;
    }

    _log.debug_P(PSTR("changing state: %s -> %s"), toString(_state), toString(newState));

    _state = newState;
}

void Pump::runStateMachine()
{
    switch (_state) {
        case State::Idle:
            break;

        case State::StartRequested:
            startIrrigation();
            break;

        case State::RampingUp:
            checkRampUpState();
            break;

        case State::Pumping:
            checkIrrigationState();
            break;

        case State::Stopping:
            stopIrrigation();
            break;

        case State::CheckLeaking:
            checkLeaking();
            break;

        case State::Error:
            break;
    }
}

const char* Pump::toString(State state)
{
    switch (state) {
        case State::Idle: return "Idle";
        case State::StartRequested: return "StartRequested";
        case State::RampingUp: return "RampingUp";
        case State::Pumping: return "Pumping";
        case State::Stopping: return "Stopping";
        case State::CheckLeaking: return "CheckLeaking";
        case State::Error: return "Error";
    }

    return "Unknown";
}

void Pump::startIrrigation()
{
    _log.info_P(
        PSTR("starting irrigation, amount=%u, manual=%s"),
        _requestedAmount,
        _manualIrrigation ? "yes" : "no"
    );

    _log.debug_P(PSTR("resetting flow sensor"));
    _flowSensor.reset();

    _log.debug_P(PSTR("opening zone %u"), _requestedZone);
    _zoneController.open(_requestedZone);

    _log.debug_P(PSTR("starting pump %u"), _id);
    _outputController.activate(Config::Pins::PumpOutputBase + _id);

    _log.debug_P(PSTR("resetting ramp-up timer"));
    _rampUpStartTicks = millis();

    _log.debug_P(PSTR("ramping up for %u ms"), Config::Pump::PumpRampUpTimeMs);

    _waterFlowErrors = 0;

    changeState(State::RampingUp);
    notifyStateChangedHandlers(true);
}

void Pump::stopIrrigation(const bool leakCheck)
{
    _log.info_P(PSTR("stopping irrigation"));

    _log.debug_P(PSTR("stopping pump"));
    _outputController.deactive(Config::Pins::PumpOutputBase + _id);

    _log.debug_P(PSTR("closing zone"));
    _zoneController.close(_requestedZone);

    if (leakCheck) {
        _lastCheckTimestamp = millis();
        _lastFlowSensorTicks = _flowSensor.ticks();

        _log.debug_P(PSTR("checking leaks for %u ms"), Config::Pump::LeakCheckLengthMs);

        changeState(State::CheckLeaking);
    } else {
        _log.info_P(PSTR("stopping without leak checking"));

        changeState(State::Idle);
    }

    notifyStateChangedHandlers(false);
}

void Pump::checkIrrigationState()
{
    if (millis() - _lastCheckTimestamp < Config::Pump::FlowSensorCheckIntervalMs)
        return;

    _lastCheckTimestamp = millis();

    const auto flowSensorTicks = _flowSensor.ticks();
    const auto flowSensorTicksDelta = flowSensorTicks - _lastFlowSensorTicks;
    _lastFlowSensorTicks = flowSensorTicks;
    _log.debug_P(PSTR("flow sensor ticks delta: %u"), flowSensorTicksDelta);

    // FIXME use TicksPerDecilitre from Settings
    const auto totalAmount = static_cast<double>(flowSensorTicks) / Config::FlowSensorTicksPerDecilitre; // _settings.data.flowSensor.ticksPerDecilitres;
    _log.debug_P(PSTR("total amount: %0.1f"), totalAmount);

    const Decilitres remainingAmount = _requestedAmount - totalAmount;
    _log.debug_P(PSTR("remaining amount: %d"), remainingAmount);

    // Check if water flow is sufficient to avoid running the pump dry
    if (flowSensorTicksDelta < Config::Pump::FlowSensorMinDeltaTicks) {
        _log.warning_P(PSTR("insufficient water flow detected"));

        if (++_waterFlowErrors >= 3) {
            _log.error_P(PSTR("stopping due to insufficient water flow"));

            if (_errorHandler) {
                _errorHandler(Error::FlowRateTooLow);
            }

            changeState(State::Stopping);
            return;
        }
    }

    // Check if the requested amount is pumped out
    if (!_manualIrrigation && totalAmount >= _requestedAmount)
    {
        _log.debug_P(PSTR("requested amount pumped out, stopping"));
        changeState(State::Stopping);
    }
}

void Pump::checkRampUpState()
{
    if (millis() - _rampUpStartTicks < Config::Pump::PumpRampUpTimeMs)
        return;

    _log.debug_P(PSTR("ramp-up finished"));

    const auto flowSensorCurrentTicks = _flowSensor.ticks();
    _log.debug_P(PSTR("flow sensor ticks after ramp-up: %u"), flowSensorCurrentTicks);
    _lastFlowSensorTicks = flowSensorCurrentTicks;

    changeState(State::Pumping);

    _lastCheckTimestamp = millis();
}

void Pump::checkLeaking()
{
    if (millis() - _lastCheckTimestamp < Config::Pump::LeakCheckLengthMs)
        return;

    const auto flowSensorTicksDelta = _flowSensor.ticks() - _lastFlowSensorTicks;
    _log.debug_P(PSTR("flow sensor ticks delta after leak checking: %u"), flowSensorTicksDelta);

    if (flowSensorTicksDelta > _settings.data.flowSensor.leakCheckDetectionTicks) {
        _log.warning_P(PSTR("leak detected!"));
        if (_errorHandler) {
            _errorHandler(Error::LeakDetected);
        }
    }

    changeState(State::Idle);
}

void Pump::notifyStateChangedHandlers(const bool running)
{
    for (const auto& h : _stateChangedHandlers) {
        if (h) {
            h(running);
        }
    }
}