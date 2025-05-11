#include <Arduino.h>
#include <WiFi.h>
#include "esp_log.h"
#include "esp_camera.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include "roverMod.h"
#include "prefsMod.h"
#include "pirMod.h"

#define DEEP_SLEEP_DURATION_MILLIS 30 * 1000

// Deep sleep duration (10 seconds in microseconds)
#define DEEP_SLEEP_DURATION_MICROS 1000 * DEEP_SLEEP_DURATION_MILLIS

// Wake duration to check for connections (milliseconds)
#define WAKE_CHECK_DURATION_MILLIS 3 * 1000

static const char *TAG = "sleepMod";
static const char *IS_SLEEP_DISABLED_KEY = "isSleepDisabled";

static int anyOneConnected = 0;

// Function to enter deep sleep
static void tryToSleep()
{
    if (readBool(IS_SLEEP_DISABLED_KEY) || anyOneConnected || ifAPModeOn() || ifMotionDetected())
    {
        ESP_LOGI(TAG, "Ignoring this sleep cycle");
        return;
    }

    ESP_LOGI(TAG, "Entering deep sleep...");

    // Configure timer wakeup for 10 seconds
    esp_sleep_enable_timer_wakeup(DEEP_SLEEP_DURATION_MICROS);

    // Enter deep sleep
    roverToSleep();
    esp_deep_sleep_start();
}

// WebSocket task function
static void sleepTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(WAKE_CHECK_DURATION_MILLIS));
        tryToSleep();
    }
}

void enableSleep()
{
    ESP_LOGI(TAG, "Toggling Sleep ON");
    saveBoolean(IS_SLEEP_DISABLED_KEY, false);
}

void disableSleep()
{
    ESP_LOGI(TAG, "Toggling Sleep OFF");
    saveBoolean(IS_SLEEP_DISABLED_KEY, true);
}

void anyClientsConnected()
{
    ESP_LOGI(TAG, "Some clients were connected");
    anyOneConnected = 1;
}

void noClientsConnected()
{
    ESP_LOGI(TAG, "No clients were connected");
    anyOneConnected = 0;
}

void setupSleepMod()
{
    ESP_LOGI(TAG, "Setting up sleep module...");
    xTaskCreate(
        sleepTask,
        "SleepTask",
        2048,
        NULL,
        5, // Priority (1-25, higher is more urgent)
        NULL);
}
