#pragma once

#include "Config.h"

#include <string>

#include <Arduino.h>
#include <StreamString.h>
#include <WiFiUdp.h>

class SystemClock;

class Logger
{
public:
    static void setup(const SystemClock& systemClock);

    explicit Logger(std::string category);

    enum class Severity
    {
        Error,
        Warning,
        Info,
        Debug
    };

    template <typename... Params>
    void log(Severity severity, const char* fmt, Params... params) const
    {
        StreamString ss;

        if (!_inBlock) {
            ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());
        }

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        if (Config::Network::SyslogEnabled && _p) {
            _p->sendToSyslogServer(Config::Network::SyslogHostName, ss.c_str());
        }

        // Print new line after sending the message to Syslog server
        if (!_inBlock) {
            ss.println();
        }

        Serial.print(ss);
    }

    template <typename... Params>
    void error(const char* fmt, Params... params) const
    {
        log(Severity::Error, fmt, params...);
    }

    template <typename... Params>
    void warning(const char* fmt, Params... params) const
    {
        log(Severity::Warning, fmt, params...);
    }

    template <typename... Params>
    void info(const char* fmt, Params... params) const
    {
        log(Severity::Info, fmt, params...);
    }

    template <typename... Params>
    void debug(const char* fmt, Params... params) const
    {
        log(Severity::Debug, fmt, params...);
    }

    struct Block
    {
        Block(bool& inBlock)
            : _inBlock(inBlock)
        {}

        ~Block()
        {
            _inBlock = false;
            Serial.println();
        }

    private:
        bool& _inBlock;
    };

    template <typename... Params>
    Block logBlock(Severity severity, const char* fmt, Params... params) const
    {
        _inBlock = true;

        StreamString ss;

        ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        Serial.print(ss);

        if (Config::Network::SyslogEnabled && _p) {
            _p->sendToSyslogServer(Config::Network::SyslogHostName, ss.c_str());
        }

        return Block{ _inBlock };
    }

    template <typename... Params>
    Block debugBlock(const char* fmt, Params... params) const
    {
        return logBlock(Severity::Debug, fmt, params...);
    }

private:
    const std::string _category;
    mutable bool _inBlock = false;

    struct Private
    {
        Private(const SystemClock& systemClock)
            : systemClock(systemClock)
        {}

        WiFiUDP udp;
        const SystemClock& systemClock;

        void sendToSyslogServer(
            const char* hostName,
            const char* message,
            const char* procId = "-",
            const char* msgid = "-"
        );
    };

    static Private* _p;

    static char severityIndicator(Severity severity);
};