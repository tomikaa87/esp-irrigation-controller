#pragma once

class PumpController
{
public:
    void start();
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    bool _running = false;
};

