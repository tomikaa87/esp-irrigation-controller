#pragma once

#include "Logger.h"
#include "Types.h"

class Settings;

class WaterTank
{
public:
    explicit WaterTank(Settings& settings);

    Decilitres capacity() const;
    Decilitres level() const;
    void refill() const;
    void use(Decilitres amount) const;

private:
    Logger _log{ "WaterTank" };
    Settings& _settings;
};

