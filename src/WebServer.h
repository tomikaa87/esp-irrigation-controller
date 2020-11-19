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
        const ApplicationConfig& appConfig,
        FlowSensor& flowSensor
    );

    using ZoneStartedHandler = std::function<bool(uint8_t zone)>;
    void setZoneStartedHandler(ZoneStartedHandler&& handler);

    using StopHandler = std::function<bool()>;
    void setStopHandler(StopHandler&& handler);

    using StartDrainingHandler = std::function<void(uint8_t zone)>;
    void setStartDrainingHandler(StartDrainingHandler&& handler);

    using StopDrainingHandler = std::function<void()>;
    void setStopDrainingHandler(StopDrainingHandler&& handler);

    void shutdown();

    void task();

private:
    Logger _log{ "WebServer" };
    const ApplicationConfig& _appConfig;
    FlowSensor& _flowSensor;

    ESP8266WebServer _server;

    ZoneStartedHandler _zoneStartedHandler;
    StopHandler _stopHandler;

    SchedulerApiController _schedulerApiController;

    StartDrainingHandler _startDrainingHandler;
    StopDrainingHandler _stopDrainingHandler;

    bool _shutdown = false;

    void onApiZoneStart(uint8_t zone);
    void onApiStop();
    void onApiStatus();

    void onSchedulerApiAdd();

    void onStartDraining(uint8_t zone);
    void onStopDraining();
};