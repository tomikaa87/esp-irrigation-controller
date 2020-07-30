#include "Config.h"
#include "FlowSensor.h"

static FlowSensor* _flowSensorInstance = nullptr;

//
// Pin falling edge interrupt latency: 5us - 18us (5.5us - 6.3us avg)
// FIXME: rising edge interrupt is buggy, ISR called continuously while the input is high
//

ICACHE_RAM_ATTR void _flowSensorIoIsr()
{
    if (_flowSensorInstance) {
        _flowSensorInstance->onIoInterruptCalled();
    }
}

FlowSensor::FlowSensor()
{
    _log.info("initializing GPIO input");

    pinMode(Config::Pins::FlowSensorInput, INPUT);
    attachInterrupt(Config::Pins::FlowSensorInput, _flowSensorIoIsr, FALLING);

    _flowSensorInstance = this;
}

FlowSensor::~FlowSensor()
{
    _flowSensorInstance = nullptr;
}

std::size_t FlowSensor::ticks() const
{
    return _ticks;
}

void FlowSensor::reset()
{
    _log.info("resetting flow sensor tick count");

    _ticks = 0;
}

void FlowSensor::task()
{
    static int32_t ts = 0;
    static decltype(_ticks) lastTicks = 0;

    const auto delta = _ticks - lastTicks;

    if (delta > 0 && millis() - ts > 1000) {
        ts = millis();
        _log.debug("ticks=%u, delta=%u", _ticks, delta);
        lastTicks = _ticks;
    }
}

ICACHE_RAM_ATTR void FlowSensor::onIoInterruptCalled()
{
    ++_ticks;
}