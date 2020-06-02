#pragma once

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
    Settings& _settings;
};

