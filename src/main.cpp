#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>

static ESP8266WebServer webServer(80);

void setup()
{
    Serial.begin(74880);

    Serial.println("Connecting to WiFi");
    WiFi.begin("", "");

    Serial.println("Setting up SPIFFS");
    SPIFFS.begin();

    Serial.println("Setting up the web server");
    webServer.serveStatic("/", SPIFFS, "/index.html");
    webServer.serveStatic("/index.html", SPIFFS, "/index.html");
    webServer.serveStatic("/jquery-3.5.1.min.js", SPIFFS, "/jquery-3.5.1.min.js");
    webServer.begin();

    Serial.println("Initialization finished");
}

void loop()
{
    webServer.handleClient();
}