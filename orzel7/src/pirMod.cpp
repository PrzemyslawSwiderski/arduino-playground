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
static const unsigned long NOTIF_COOLDOWN = 5 * 60000; // 5 minutes notification cooldown

static int prevPirState = 1;
static int motionDetected = 0;
static unsigned long lastNotifTime = 10000000;

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

static void handleMotionDetection()
{
    motionDetected = 1;

    ESP_LOGI(TAG, "Motion detected!");
    if (!readBool(IS_PIR_ON_KEY))
    {
        ESP_LOGI(TAG, "Skipping Notfication, notification sending is OFF.");
        motionDetected = 0;
        return;
    }

    unsigned long currentTime = millis();
    if (!isTimeAfter(currentTime, lastNotifTime, NOTIF_COOLDOWN))
    {
        ESP_LOGI(TAG, "Skipping Notfication, notification was sent recently.");
        motionDetected = 0;
        return;
    }

    ESP_LOGI(TAG, "Sending notification...");
    ledOn();
    sendNofication();
    vTaskDelay(pdMS_TO_TICKS(5000));
    ledOff();
    lastNotifTime = millis();

    motionDetected = 0;
}

static void pirSensorTask(void *pvParameters)
{
    while (1)
    {
        int pirState = digitalRead(PIR_SENSOR_PIN);
        ESP_LOGI(TAG, "PIR state: %d", pirState);
        if (prevPirState == 0 && pirState == 1)
        {
            handleMotionDetection();
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
