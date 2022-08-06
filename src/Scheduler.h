#pragma once

#include "Config.h"
#include "Logger.h"
#include "Time.hpp"
#include "Types.h"

#include <bitset>
#include <cstdint>
#include <ctime>
#include <queue>
#include <vector>

#include <iostream>

class ISystemClock;

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

    Scheduler(const ISystemClock& systemClock);

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
    const ISystemClock& _systemClock;
    std::queue<PendingEvent> _pendingEvents;
    std::vector<StoredEvent> _storedEvents;

    void logStoredEvent(const StoredEvent& e) const;
    void logPendingEvent(const PendingEvent& e) const;
};