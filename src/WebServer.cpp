#include "Config.h"
#include "WebServer.h"

#include <FS.h>

WebServer::WebServer(const int port)
    : _server(port)
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

void WebServer::task()
{
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
    _log.info("status requested");

    // TODO
    _server.send(200, "application/json", R"({"activeZone":null,"tankLevel":0,"firmwareVersion":"0.0.0"})");
}