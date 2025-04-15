#include "pirMod.h"
#include <Arduino.h>
#include "esp_log.h"
#include "pins.h"
#include "roverMod.h"

static const char *TAG = "pirMod";

// Checks if motion was detected, sets LED HIGH and starts a timer
static void detectsMovement()
{
    ESP_EARLY_LOGI(TAG, "Motion detected! Taking photo...");
    ledOn();
    delay(5000);
    ledOff();
}

void setupPirMod()
{
    pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
    // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode

    ESP_LOGI(TAG, "Waiting for PIR sensor to stabilize (30s)...");
    delay(30000); // HC-SR501 needs ~30-60s to stabilize after power-on
    ESP_LOGI(TAG, "Stabilization completed");
    attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), detectsMovement, RISING);
}
