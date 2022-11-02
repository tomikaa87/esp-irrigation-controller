#include "Scheduler.h"

#include <ISystemClock.h>

Scheduler::Scheduler(const ISystemClock& systemClock)
    : _systemClock(systemClock)
{
}

void Scheduler::task()
{
    const auto t = _systemClock.localTime();
    auto* const tm = std::localtime(&t);
    const auto minutesFromMidnight = tm->tm_hour * 60 + tm->tm_min;

    for (auto& e : _storedEvents)
    {
        if (!e.active)
            continue;

        _log.debug_P(PSTR("processing active event:"));
        logStoredEvent(e);

        if (!e.days.test(tm->tm_wday))
        {
            _log.debug_P(PSTR("event is not scheduled for today"));
            continue;
        }

        if (!e.executed && e.minutesFromMidnight == minutesFromMidnight)
        {
            _log.debug_P(PSTR("executing event"));

            e.executed = true;

            // Create separate irrigation events for each active zone
            for (uint8_t zone = 0; zone < e.zones.size(); ++zone)
            {
                if (e.zones.test(zone))
                {
                    PendingEvent pe{ e.amount, zone };
                    _log.debug_P(PSTR("creating pending event:"));
                    logPendingEvent(pe);
                    _pendingEvents.push(pe);
                }
            }
        }
        else if (e.executed && e.minutesFromMidnight < minutesFromMidnight)
        {
            _log.debug_P(PSTR("marking event as executable"));
            e.executed = false;
        }
    }
}

void Scheduler::logStoredEvent(const StoredEvent& e) const
{
    // '}' is closed in the last log line
    const auto block = _log.debugBlock_P(PSTR("ScheduledEvent{"));

    _log.debug_P(PSTR("zones=["));
    bool first = true;
    for (uint8_t i = 0; i < e.zones.size(); ++i) {
        if (e.zones.test(i)) {
            _log.debug_P(PSTR("%s%u"), !first ? "," : "", i);
            first = false;
        }
    }

    _log.debug_P(PSTR("], minutesFromMidnight=%u"), e.minutesFromMidnight);

    _log.debug_P(PSTR(", days=["));
    first = true;
    for (uint8_t i = 0; i < e.days.size(); ++i)
    {
        static const char* Days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
        if (e.days.test(i)) {
            _log.debug_P(PSTR("%s%s"), !first ? "," : "", Days[i]);
        }
    }

    _log.debug_P(PSTR("], amount=%u, active=%s, executed=%s}"),
        e.amount,
        e.active ? "Yes" : "No",
        e.executed ? "Yes" : "No"
    );
}

void Scheduler::logPendingEvent(const PendingEvent& e) const
{
    _log.debug_P(
        PSTR("PendingEvent{zone=%u, amount=%u}"),
        e.zone,
        e.amount
    );
}