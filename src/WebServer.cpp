#include "Config.h"
#include "FlowSensor.h"
#include "WebServer.h"

#include <LittleFS.h>
#include <vector>

WebServer::WebServer(
    const ApplicationConfig& appConfig,
    FlowSensor& flowSensor
)
    : _appConfig(appConfig)
    , _flowSensor(flowSensor)
    , _server(appConfig.webServer.port)
{
    _log.info("setting up static contents");

    _server.serveStatic("/", LittleFS, "/index.html");
    _server.serveStatic("/index.html", LittleFS, "/index.html");
    _server.serveStatic("/jquery-3.6.0.min.js", LittleFS, "/jquery-3.6.0.min.js");

    _log.info("setting up API controller");

    _server.on("/api/zone/0/start", [this] { onApiZoneStart(0); });
    _server.on("/api/zone/1/start", [this] { onApiZoneStart(1); });
    _server.on("/api/zone/2/start", [this] { onApiZoneStart(2); });
    _server.on("/api/zone/3/start", [this] { onApiZoneStart(3); });
    _server.on("/api/zone/4/start", [this] { onApiZoneStart(4); });
    _server.on("/api/zone/5/start", [this] { onApiZoneStart(5); });

    _server.on("/api/zone/stop", [this] { onApiStop(); });

    _server.on("/api/status", [this] { onApiStatus(); });

    _server.on("/api/scheduler/add", [this] { onSchedulerApiAdd(); });

    _server.on("/api/drain/0/start", [this] { onStartDraining(0); });
    _server.on("/api/drain/1/start", [this] { onStartDraining(1); });
    _server.on("/api/drain/2/start", [this] { onStartDraining(2); });
    _server.on("/api/drain/3/start", [this] { onStartDraining(3); });
    _server.on("/api/drain/4/start", [this] { onStartDraining(4); });
    _server.on("/api/drain/5/start", [this] { onStartDraining(5); });

    _server.on("/api/zone/0/enqueue/stored", [this] { onApiEnqueueStored(0); });
    _server.on("/api/zone/1/enqueue/stored", [this] { onApiEnqueueStored(1); });
    _server.on("/api/zone/2/enqueue/stored", [this] { onApiEnqueueStored(2); });
    _server.on("/api/zone/3/enqueue/stored", [this] { onApiEnqueueStored(3); });
    _server.on("/api/zone/4/enqueue/stored", [this] { onApiEnqueueStored(4); });
    _server.on("/api/zone/5/enqueue/stored", [this] { onApiEnqueueStored(5); });

    _server.on("/api/drain/stop", [this] { onStopDraining(); });

    _server.begin();
}

void WebServer::setZoneStartedHandler(ZoneStartedHandler&& handler)
{
    _zoneStartedHandler = std::move(handler);
}

void WebServer::setStopHandler(StopHandler&& handler)
{
    _stopHandler = std::move(handler);
}

void WebServer::setStartDrainingHandler(StartDrainingHandler&& handler)
{
    _startDrainingHandler = std::move(handler);
}

void WebServer::setStopDrainingHandler(StopDrainingHandler&& handler)
{
    _stopDrainingHandler = std::move(handler);
}

void WebServer::setEnqueueStoredHandler(EnqueueStoredHandler&& handler)
{
    _enqueueStoredHandler = std::move(handler);
}

void WebServer::shutdown()
{
    if (_shutdown) {
        return;
    }

    _log.info("shutting down");

    _server.close();
    LittleFS.end();

    _shutdown = true;
}

void WebServer::task()
{
    if (_shutdown) {
        return;
    }

    _server.handleClient();
}

void WebServer::onApiZoneStart(const uint8_t zone)
{
    _log.info("zone start requested, zone=%u", zone);

    if (zone >= Config::Zones) {
        _server.send(400, "text/plain", "No such zone");
        return;
    }

    if (!_zoneStartedHandler) {
        _log.error("zone started handler not registered");
        _server.send(500, "text/plain", "Missing zone started handler");
        return;
    }

    if (!_zoneStartedHandler(zone)) {
        _server.send(400, "Can't start manual irrigation");
        return;
    }

    _server.send(204);
}

void WebServer::onApiStop()
{
    _log.info("stop requested");

    if (!_stopHandler) {
        _log.error("stop handler not registered");
        _server.send(500, "text/plain", "Missing stop handler");
        return;
    }

    if (!_stopHandler()) {
        _server.send(400, "Can't stop irrigation");
        return;
    }

    _server.send(204);
}

void WebServer::onApiStatus()
{
#if 0
    _log.info("status requested");
#endif

    auto payload = nlohmann::json{
        { "activeZone", 0 },
        { "tankLevel", 0 },
        { "firmwareVersion", _appConfig.firmwareVersion.toString() },
        { "flowSensor", {
            { "ticks", _flowSensor.ticks() }
        }}
    };

    // TODO
    _server.send(200, "application/json", payload.dump().c_str());
}

void WebServer::onSchedulerApiAdd()
{
    if (_server.method() != HTTPMethod::HTTP_POST) {
        _server.send(405);
        return;
    }

    const auto body = nlohmann::json::parse(_server.arg(0).c_str());

    std::vector<SchedulerApiController::Event> events;
    const auto result = _schedulerApiController.parseInputJson(body, events);

    if (result != SchedulerApiController::ParseResult::Ok) {
        _server.send(400, "text/plain", SchedulerApiController::toString(result));
    }
}

void WebServer::onStartDraining(const uint8_t zone)
{
    if (!_startDrainingHandler) {
        _server.send(500);
        return;
    }

    if (zone > Config::Zones - 1) {
        _server.send(400);
        return;
    }

    _startDrainingHandler(zone);
    _server.send(200);
}

void WebServer::onStopDraining()
{
    if (!_stopDrainingHandler) {
        _server.send(500);
        return;
    }

    _stopDrainingHandler();
    _server.send(200);
}

void WebServer::onApiEnqueueStored(const uint8_t zone)
{
    if (!_enqueueStoredHandler) {
        _server.send(500);
        return;
    }

    _enqueueStoredHandler(zone);
    _server.send(200);
}