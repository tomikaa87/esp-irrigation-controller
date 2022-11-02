#include "SchedulerApiController.h"

#include <string>

static const char* DayShortNames[] = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat"
};

SchedulerApiController::ParseResult SchedulerApiController::parseInputJson(const nlohmann::json& in, std::vector<Event>& events)
{
    if (in.is_null()) {
        _log.warning_P(PSTR("input json is null"));
        return ParseResult::InvalidInput;
    }

    if (in.is_object()) {
        _log.debug_P(PSTR("parsing single input object"));
        Event event;
        const auto result = parseInputObject(in, event);
        if (result != ParseResult::Ok) {
            return result;
        }
        events.push_back(std::move(event));
        return ParseResult::Ok;
    }

    if (in.is_array()) {
        _log.debug_P(PSTR("parsing multiple input objects"));
        for (const auto& obj : in) {
            Event event;
            const auto result = parseInputObject(obj, event);
            if (result != ParseResult::Ok) {
                return result;
            }
            events.push_back(std::move(event));
        }
        return ParseResult::Ok;
    }

    _log.warning_P(PSTR("invalid input JSON"));

    return ParseResult::InvalidInput;
}

SchedulerApiController::ParseResult SchedulerApiController::parseDaysArray(const nlohmann::json& in, WeekdaysSet& days)
{
    for (const auto& day : in) {
        if (!day.is_string()) {
            _log.warning_P(PSTR("invalid days array: string expected"));
            return ParseResult::InvalidDaysArray;
        }

        auto ds = day.get<std::string>();
        std::transform(ds.begin(), ds.end(), ds.begin(), [](char c) { return std::tolower(c); });

        bool found = false;
        for (auto i = 0; i < 7; ++i) {
            if (strncmp(DayShortNames[i], ds.c_str(), 3) != 0) {
                continue;
            }

            days.set(i);
            found = true;
        }

        if (!found) {
            return ParseResult::InvalidDaysArray;
        }
    }

    return ParseResult::Ok;
}

SchedulerApiController::ParseResult SchedulerApiController::parseZonesArray(const nlohmann::json& in, ZoneSet& zones)
{
    for (const auto& zone : in) {
        if (!zone.is_number_integer()) {
            _log.warning_P(PSTR("invalid zones array: integer expected"));
            return ParseResult::InvalidZonesArray;
        }

        const auto zi = zone.get<unsigned>();
        if (zi >= Config::Zones) {
            _log.warning_P(PSTR("invalid zones array: invalid zone number"));
            return ParseResult::InvalidZonesArray;
        }

        zones.set(zi);
    }

    return ParseResult::Ok;
}

SchedulerApiController::ParseResult SchedulerApiController::parseInputObject(const nlohmann::json& in, Event& output)
{
    if (!in.contains("event")) {
        _log.warning_P(PSTR("invalid object: event is missing"));
        return ParseResult::InvalidInputObject;
    }

    const auto& event = in["event"];

    if (!event.contains("amount")) {
        _log.warning_P(PSTR("invalid event object: amount field is missing"));
        return ParseResult::InvalidInputObject;
    }

    if (!event.contains("zones")) {
        _log.warning_P(PSTR("invalid event object: zones field is missing"));
        return ParseResult::InvalidInputObject;
    }

    if (!event.contains("days")) {
        _log.warning_P(PSTR("invalid event object: zones field is missing"));
        return ParseResult::InvalidInputObject;
    }

    const auto& amount = event["amount"];
    const auto& zones = event["zones"];
    const auto& days = event["days"];

    if (!amount.is_number_integer()) {
        _log.warning_P(PSTR("invalid event object: amount must be and integer value"));
        return ParseResult::InvalidInputObject;
    }

    if (!zones.is_array()) {
        _log.warning_P(PSTR("invalid event object: zones must be an array"));
        return ParseResult::InvalidInputObject;
    }

    if (!days.is_array()) {
        _log.warning_P(PSTR("invalid event object: days must be an array"));
        return ParseResult::InvalidInputObject;
    }

    WeekdaysSet daySet;
    auto result = parseDaysArray(days, daySet);
    if (result != ParseResult::Ok) {
        return result;
    }

    ZoneSet  zoneSet;
    result = parseZonesArray(zones, zoneSet);
    if (result != ParseResult::Ok) {
        return result;
    }

    output.amount = amount.get<decltype(Event::amount)>();
    output.days = std::move(daySet);
    output.zones = std::move(zoneSet);

    return ParseResult::Ok;
}

const char* SchedulerApiController::toString(const ParseResult pr)
{
    switch (pr) {
        case ParseResult::Ok:
            return "Ok";
        case ParseResult::InvalidInput:
            return "InvalidInput";
        case ParseResult::InvalidInputObject:
            return "InvalidInputObject";
        case ParseResult::InvalidDaysArray:
            return "InvalidDaysArray";
        case ParseResult::InvalidZonesArray:
            return "InvalidZonesArray";
    }

    return "";
}