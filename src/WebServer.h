#pragma once

#include "Logger.h"
#include "SchedulerApiController.h"

#include <functional>

#include <ESP8266WebServer.h>

class WebServer
{
public:
    WebServer(int port = 80);

    using ZoneStartedHandler = std::function<bool(uint8_t zone)>;
    void setZoneStartedHandler(ZoneStartedHandler&& handler);

    using StopHandler = std::function<bool()>;
    void setStopHandler(StopHandler&& handler);

    void task();

private:
    Logger _log{ "WebServer" };

    ESP8266WebServer _server;

    ZoneStartedHandler _zoneStartedHandler;
    StopHandler _stopHandler;

    SchedulerApiController _schedulerApiController;

    void onApiZoneStart(uint8_t zone);
    void onApiStop();
    void onApiStatus();

    void onSchedulerApiAdd();
};