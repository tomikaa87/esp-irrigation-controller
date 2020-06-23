#include "Config.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>

//
// Pin falling edge interrupt latency: 5us - 18us (5.5us - 6.3us avg)
// FIXME: rising edge interrupt is buggy, IRS called continuously while the input is high
//

static ESP8266WebServer webServer(80);

ICACHE_RAM_ATTR void isr()
{
    digitalWrite(D7, HIGH);
    digitalWrite(D7, LOW);
}

void setup()
{
    Serial.begin(74880);

    Serial.println("Connecting to WiFi");
    WiFi.begin(Config::WiFiSSID, Config::WiFiPassword);

    Serial.println("Setting up SPIFFS");
    SPIFFS.begin();

    Serial.println("Setting up the web server");
    webServer.serveStatic("/", SPIFFS, "/index.html");
    webServer.serveStatic("/index.html", SPIFFS, "/index.html");
    webServer.serveStatic("/jquery-3.5.1.min.js", SPIFFS, "/jquery-3.5.1.min.js");
    webServer.begin();

    Serial.println("Initialization finished");

    pinMode(D7, OUTPUT);
    digitalWrite(D7, LOW);

    pinMode(D8, INPUT);
    attachInterrupt(D8, isr, FALLING);
}

void loop()
{
    webServer.handleClient();
}