#pragma once

#include "Logger.h"
#include "SchedulerApiController.h"

#include <functional>

#include <ESP8266WebServer.h>

class FlowSensor;

class WebServer
{
public:
    WebServer(
        FlowSensor& flowSensor,
        int port = 80
    );

    using ZoneStartedHandler = std::function<bool(uint8_t zone)>;
    void setZoneStartedHandler(ZoneStartedHandler&& handler);

    using StopHandler = std::function<bool()>;
    void setStopHandler(StopHandler&& handler);

    void shutdown();

    void task();

private:
    Logger _log{ "WebServer" };
    FlowSensor& _flowSensor;

    ESP8266WebServer _server;

    ZoneStartedHandler _zoneStartedHandler;
    StopHandler _stopHandler;

    SchedulerApiController _schedulerApiController;

    bool _shutdown = false;

    void onApiZoneStart(uint8_t zone);
    void onApiStop();
    void onApiStatus();

    void onSchedulerApiAdd();
};