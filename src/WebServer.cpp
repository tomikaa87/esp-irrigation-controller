#include "Config.h"
#include "FirmwareVersion.h"
#include "FlowSensor.h"
#include "WebServer.h"

#include <FS.h>
#include <vector>

WebServer::WebServer(
    FlowSensor& flowSensor,
    const int port
)
    : _server(port)
    , _flowSensor(flowSensor)
{
    _log.info("setting up static contents");

    _server.serveStatic("/", SPIFFS, "/index.html");
    _server.serveStatic("/index.html", SPIFFS, "/index.html");
    _server.serveStatic("/jquery-3.5.1.min.js", SPIFFS, "/jquery-3.5.1.min.js");

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

void WebServer::shutdown()
{
    if (_shutdown) {
        return;
    }

    _log.info("shutting down");

    _server.close();
    SPIFFS.end();

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

    static const auto fwVer =
        std::to_string(FW_VER_MAJOR) + "."
        + std::to_string(FW_VER_MINOR) + "."
        + std::to_string(FW_VER_PATCH);

    auto payload = nlohmann::json{
        { "activeZone", 0 },
        { "tankLevel", 0 },
        { "firmwareVersion", fwVer },
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