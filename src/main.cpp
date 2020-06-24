#include "Config.h"
#include "IrrigationController.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <FS.h>

static std::unique_ptr<IrrigationController> irrigationController;

void ICACHE_RAM_ATTR timer1Isr()
{
    if (irrigationController) {
        irrigationController->epochTimerIsr();
    }
}

void initializeEpochTimer()
{
    timer1_isr_init();
    timer1_attachInterrupt(timer1Isr);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    timer1_write(100);
}

void initializeSerial()
{
    Serial.begin(74880);
}

void initWiFi()
{
    Serial.println("Connecting to WiFi");
    WiFi.begin(Config::WiFiSSID, Config::WiFiPassword);
}

void initFileSystem()
{
    Serial.println("Setting up SPIFFS");
    SPIFFS.begin();
}

void setup()
{
    initializeEpochTimer();
    initializeSerial();
    initWiFi();
    initFileSystem();

    irrigationController.reset(new IrrigationController);

    Serial.println("Initialization finished");
}

void loop()
{
    if (irrigationController)
        irrigationController->task();
}