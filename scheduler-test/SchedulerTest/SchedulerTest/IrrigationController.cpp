#include "IrrigationController.h"

IrrigationController::IrrigationController()
    : _waterTank{ _settings }
{
    _settings.load();
    _zoneController.closeAll();
}

void IrrigationController::task()
{
    _flowSensor.task();

    if (_taskCallCount++ % 10 == 0)
    {
        _scheduler.task();
        processPendingEvents();
    }

    runStateMachine();
}

void IrrigationController::processPendingEvents()
{
    if (_state != State::Idle || !_scheduler.hasPendingEvents() || _pumpController.isRunning())
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
            _pumpController.start();
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
                    _pumpController.stop();
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
            if (pumpedAmount >= _requiredAmount)
            {
                std::cout << "the required amount has been pumped out" << std::endl;
                _state = State::Stopping;
            }
            break;
        }

        case State::Stopping:
        {
            std::cout << "stopping the irrigation cycle" << std::endl;
            _pumpController.stop();
            _zoneController.closeAll();
            const Decilitres pumpedAmount = _flowSensor.ticks() / _settings.data.flowSensor.ticksPerDecilitres;
            _waterTank.use(pumpedAmount);
            _state = State::Idle;
            break;
        }
    }
}
