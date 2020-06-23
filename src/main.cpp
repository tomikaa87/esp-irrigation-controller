#include "Config.h"
#include "IrrigationController.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <FS.h>

static std::unique_ptr<IrrigationController> irrigationController;

void setup()
{
    Serial.begin(74880);

    Serial.println("Connecting to WiFi");
    WiFi.begin(Config::WiFiSSID, Config::WiFiPassword);

    Serial.println("Setting up SPIFFS");
    SPIFFS.begin();

    irrigationController.reset(new IrrigationController);

    Serial.println("Initialization finished");
}

void loop()
{
    if (irrigationController)
        irrigationController->task();
}