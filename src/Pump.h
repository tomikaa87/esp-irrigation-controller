#pragma once

#include "Logger.h"
#include "Types.h"

#include <functional>
#include <set>

class FlowSensor;
class OutputController;
class Settings;
class ZoneController;

class Pump
{
public:
    explicit Pump(
        unsigned id,
        std::set<uint8_t> zones,
        FlowSensor& flowSensor,
        OutputController& outputController,
        ZoneController& zoneController,
        const Settings& settings
    );

    int id() const;

    void task();

    bool start(uint8_t zone);
    bool start(uint8_t zone, Decilitres amount);
    void stop();

    bool containsZone(uint8_t zone) const;
    bool isRunning() const;
    Decilitres pumpedAmount() const;
    Decilitres remainingAmount() const;
    uint8_t activeZone() const;
    bool isManual() const;

    using StateChangedHandler = std::function<void (bool running)>;
    void addStateChangedHandler(StateChangedHandler&& handler);

    enum class Error {
        LeakDetected,
        FlowRateTooLow,
        Busy
    };

    using ErrorHandler = std::function<void (Error error)>;
    void setErrorHandler(ErrorHandler&& handler);

private:
    Logger _log;
    const unsigned _id;
    const std::set<uint8_t> _zones;
    FlowSensor& _flowSensor;
    OutputController& _outputController;
    ZoneController& _zoneController;
    const Settings& _settings;

    std::vector<StateChangedHandler> _stateChangedHandlers;
    ErrorHandler _errorHandler;

    enum class State
    {
        Idle,
        StartRequested,
        RampingUp,
        Pumping,
        Stopping,
        CheckLeaking,
        Error
    } _state = State::Idle;

    uint8_t _requestedZone = 0;
    Decilitres _requestedAmount = 0;
    bool _manualIrrigation = false;
    int32_t _rampUpStartTicks = 0;
    int32_t _lastCheckTimestamp = 0;
    std::size_t _lastFlowSensorTicks = 0;
    uint8_t _waterFlowErrors = 0;

    const char* toString(State state);

    void changeState(State newState);

    void runStateMachine();
    void startIrrigation();
    void stopIrrigation(bool leakCheck = true);
    void checkIrrigationState();
    void checkRampUpState();
    void checkLeaking();

    void notifyStateChangedHandlers(bool running);
};