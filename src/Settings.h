#pragma once

#include "Types.h"

#include <ISettingsHandler.h>

class Settings
{
public:
    explicit Settings(ISettingsHandler& handler);

    DECLARE_SETTINGS_STRUCT(TankSettings)
    {
        Decilitres capacity = 1500;
        Decilitres level = 0;
    };

    DECLARE_SETTINGS_STRUCT(FlowSensorSettings)
    {
        uint16_t ticksPerDecilitres = 46;
        uint16_t errorDetectionTicksDelta = 10;
        uint16_t leakCheckDetectionTicks = 0;
    };

    DECLARE_SETTINGS_STRUCT(IrrigationSettings)
    {
        Decilitres amounts[Config::Zones] = { 0 };
    };

    DECLARE_SETTINGS_STRUCT(Data)
    {
        TankSettings tank;
        FlowSensorSettings flowSensor;
        IrrigationSettings irrigation;
    };

    Data data;

    bool load();
    bool save();

    void loadDefaults();

private:
    ISettingsHandler& _handler;
};
