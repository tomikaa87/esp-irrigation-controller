#pragma once

#include "Config.h"
#include "Logger.h"
#include "Time.h"
#include "Types.h"

#include <bitset>
#include <cstdint>
#include <ctime>
#include <queue>
#include <vector>

#include <iostream>

class Scheduler
{
public:
    struct StoredEvent
    {
        ZoneSet zones;
        std::uint16_t minutesFromMidnight = 0;
        WeekdaysSet days = 0;
        Decilitres amount = 0;
        bool active = false;
        bool executed = false;
    };

    struct PendingEvent
    {
        Decilitres amount = 0;
        uint8_t zone = 0;

        PendingEvent(const Decilitres amount, const uint8_t zone)
            : amount{ amount }
            , zone{ zone }
        {}
    };

    Scheduler();

    bool hasPendingEvents() const
    {
        return !_pendingEvents.empty();
    }

    PendingEvent nextPendingEvent()
    {
        const auto pe = _pendingEvents.front();
        _pendingEvents.pop();
        return pe;
    }

    void task();

private:
    Logger _log{ "Scheduler" };
    std::queue<PendingEvent> _pendingEvents;
    std::vector<StoredEvent> _storedEvents;
};

inline std::ostream& operator<<(std::ostream& s, const Scheduler::StoredEvent& e)
{
    s << "StoredEvent{ ";

    s << "zones=";
    for (uint8_t i = 0; i < e.zones.size(); ++i)
    {
        if (e.zones.test(i))
            s << static_cast<int>(i) << ",";
    }

    s << " minutesFromMidnight=" << e.minutesFromMidnight;

    s << " days=";
    for (uint8_t i = 0; i < e.days.size(); ++i)
    {
        static const char* Days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
        if (e.days.test(i))
            s << Days[i] << ",";
    }

    s << " amount=" << e.amount
        << ", active=" << e.active
        << ", executed=" << e.executed;

    s << " }";

    return s;
}

inline std::ostream& operator<<(std::ostream& s, const Scheduler::PendingEvent& e)
{
    s << "PendingEvent{ "
        << "amount=" << e.amount
        << ", zone=" << static_cast<int>(e.zone)
        << " }";

    return s;
}