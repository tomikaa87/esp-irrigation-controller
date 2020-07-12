#pragma once

#include "Logger.h"
#include "Types.h"

#ifdef B1
#undef B1
#endif

#include "json.hpp"

class SchedulerApiController
{
public:
    enum class ParseResult {
        Ok,
        InvalidInput,
        InvalidInputObject,
        InvalidDaysArray,
        InvalidZonesArray
    };

    struct Event {
        Decilitres amount = 0;
        WeekdaysSet days;
        ZoneSet zones;
    };

    ParseResult parseInputJson(const nlohmann::json& in, std::vector<Event>& events);

    static const char* toString(ParseResult pr);

private:
    Logger _log{ "SchedulerApiController" };

    ParseResult parseInputObject(const nlohmann::json& in, Event& output);
    ParseResult parseDaysArray(const nlohmann::json& in, WeekdaysSet& days);
    ParseResult parseZonesArray(const nlohmann::json& in, ZoneSet& zones);
};