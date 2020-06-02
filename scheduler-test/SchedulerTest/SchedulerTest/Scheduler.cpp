#include "Scheduler.h"

#include <iostream>

Scheduler::Scheduler()
{
    StoredEvent e;
    e.amount = 15;
    //e.zones.set(0);
    e.zones.set(2);
    e.days.set(2);
    e.minutesFromMidnight = 17 * 60 + 49;
    e.active = true;

    _storedEvents.push_back(e);
}

void Scheduler::task()
{
    const auto t = std::time(nullptr);
    auto* const tm = std::localtime(&t);
    const auto minutesFromMidnight = tm->tm_hour * 60 + tm->tm_min;

    for (auto& e : _storedEvents)
    {
        std::cout << "stored event: " << e << std::endl;

        if (!e.active)
            continue;

        std::cout << "processing active event" << std::endl;

        if (!e.days.test(tm->tm_wday))
        {
            std::cout << "event is not scheduled for today" << std::endl;
            continue;
        }

        if (!e.executed && e.minutesFromMidnight == minutesFromMidnight)
        {
            std::cout << "executing event" << std::endl;

            e.executed = true;

            // Create separate irrigation events for each active zone
            for (uint8_t zone = 0; zone < e.zones.size(); ++zone)
            {
                if (e.zones.test(zone))
                {
                    PendingEvent pe{ e.amount, zone };
                    std::cout << "creating irrigation event: " << pe << std::endl;
                    _pendingEvents.push(pe);
                }
            }
        }
        else if (e.executed && e.minutesFromMidnight < minutesFromMidnight)
        {
            std::cout << "marking event as executable" << std::endl;
            e.executed = false;
        }
    }
}
