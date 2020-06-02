#pragma once

#include "Types.h"

#include <ostream>

#define SETTINGS_PACKED

class Settings
{
public:
#ifdef WIN32
#pragma pack(push, 1)
#endif
    struct Data
    {
        struct 
        {
            Decilitres capacity = 0;
            Decilitres level = 0;
        } tank;

        struct
        {
            uint16_t ticksPerDecilitres = 0;
            uint16_t errorDetectionTicks = 0;
        } flowSensor;
    } SETTINGS_PACKED data;
#ifdef WIN32
#pragma pack(pop)
#endif

    void loadDefaults();
    void load();
    void save() const;
};

inline std::ostream& operator<<(std::ostream& s, const Settings::Data& d)
{
    s << "Settings::Data{" << std::endl
        << "  tank.capacity=" << d.tank.capacity << std::endl
        << "  tank.level=" << d.tank.level << std::endl
        << "  flowSensor.ticksPerDecilitres=" << d.flowSensor.ticksPerDecilitres << std::endl
        << "}";

    return s;
}