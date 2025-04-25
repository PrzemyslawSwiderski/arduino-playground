#include "pirMod.h"
#include <Arduino.h>
#include "esp_log.h"
#include "pins.h"
#include "roverMod.h"

// Sensor model: HC-SR501
static const char *TAG = "pirMod";

// Checks if motion was detected, sets LED HIGH and starts a timer
static void detectsMovement()
{
    ESP_EARLY_LOGI(TAG, "Motion detected! Taking photo...");
    ledOn();
    delay(5000);
    ledOff();
}

static void pirSensorTask(void *pvParameters)
{
    while (1)
    {
        int pirState = digitalRead(PIR_SENSOR_PIN);
        ESP_LOGI(TAG, "PIR state: %d", pirState);
        vTaskDelay(pdMS_TO_TICKS(500)); // Yield to other tasks
    }
}

void setupPirMod()
{
    pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);

    // Create WebSocket task
    xTaskCreate(
        pirSensorTask,
        "PirSensorTask",
        2048,
        NULL,
        5, // Priority (1-25, higher is more urgent)
        NULL);
}
