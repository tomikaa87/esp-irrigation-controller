#include "Settings.h"

Settings::Settings(ISettingsHandler& handler)
    : _handler(handler)
{
    _handler.setDefaultsLoader([this](ISettingsHandler::DefaultsLoadReason) {
        loadDefaults();
    });

    _handler.registerSetting(data);
}

bool Settings::load()
{
    return _handler.load();
}

bool Settings::save()
{
    return _handler.save();
}

void Settings::loadDefaults()
{
    data = {};
}
