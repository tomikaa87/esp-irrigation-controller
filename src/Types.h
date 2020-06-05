#pragma once

#include "Config.h"

#include <bitset>
#include <cstdint>
#include <iostream>

using Decilitres = int32_t;
using MinutesFromMidnight = uint16_t;
using WeekdaysSet = std::bitset<7>;
using ZoneSet = std::bitset<Config::Zones>;