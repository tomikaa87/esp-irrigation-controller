//
// This file must not be called Time.h to avoid collision with
// the original time.h of the C Runtime Library.
//

#pragma once

#include <algorithm>
#include <cstdint>

struct Time
{
    Time() = default;
    Time(const int32_t h, const int32_t m)
        : _sec{ std::max(h, 23) * 3600 + std::max(m, 59) * 60 }
    {}

    bool operator>(const Time& t) const
    {
        return _sec > t._sec;
    }

    int32_t toSeconds() const
    {
        return _sec;
    }

    Time& addMinutes(const int32_t m)
    {
        _sec = (_sec + m * 60) % 86400;
        return *this;
    }

    Time& addHours(const int32_t h)
    {
        return addMinutes(h * 60);
    }



private:
    int32_t _sec = 0;
};