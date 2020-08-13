#include "IrrigationController.h"

#include <Arduino.h>

static std::unique_ptr<IrrigationController> irrigationController;

void setup()
{
    irrigationController.reset(new IrrigationController);

    // FMeasured = 8194, FIdeal = 8192, TRIMVAL = -240
    // rtc::setDigitalTrimming(240 >> 1);

    Serial.println("Initialization finished");
}

void loop()
{
    if (irrigationController)
        irrigationController->task();
}