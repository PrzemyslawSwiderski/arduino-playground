#include "pirMod.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "esp_log.h"
#include "pins.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "prefsMod.h"
#include "secrets.h"

// Sensor model: HC-SR501
static const char *TAG = "pirMod";

static const char *NOTIF_MSG = "Orzel7 detected movement!!!";
static const char *IS_PIR_ON_KEY = "isPirOn";

static int prevPirState = 1;
static int motionDetected = 0;

bool ifMotionDetected()
{
    return motionDetected;
}

void switchPirOn()
{
    ESP_LOGI(TAG, "Toggling PIR ON");
    saveBoolean(IS_PIR_ON_KEY, true);
}

void switchPirOff()
{
    ESP_LOGI(TAG, "Toggling PIR OFF");
    saveBoolean(IS_PIR_ON_KEY, false);
}

static void sendNofication()
{
    HTTPClient http;
    http.begin(NOTIF_URL);
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode = http.POST(NOTIF_MSG);
    if (httpResponseCode > 0)
    {
        ESP_LOGI(TAG, "HTTP Response code: %d", httpResponseCode);
    }
    else
    {
        ESP_LOGE(TAG, "Error: %s", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}

// Checks if motion was detected, sets LED HIGH and starts a timer
static void detectsMovement()
{
    motionDetected = 1;
    ledOn();
    sendNofication();
    vTaskDelay(pdMS_TO_TICKS(5000));
    ledOff();
    motionDetected = 0;
}

static void pirSensorTask(void *pvParameters)
{
    while (1)
    {
        int pirState = digitalRead(PIR_SENSOR_PIN);
        ESP_LOGI(TAG, "PIR state: %d", pirState);
        if (readBool(IS_PIR_ON_KEY) && prevPirState == 0 && pirState == 1)
        {
            unsigned long currentTime = millis();
            ESP_LOGI(TAG, "Motion detected! Sending notification...");
            detectsMovement();
        }
        prevPirState = pirState;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void setupPirMod()
{
    pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);

    // Create WebSocket task
    xTaskCreate(
        pirSensorTask,
        "PirSensorTask",
        4096,
        NULL,
        5, // Priority (1-25, higher is more urgent)
        NULL);
}
