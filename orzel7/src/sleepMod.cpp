#include <Arduino.h>
#include <WiFi.h>
#include "esp_log.h"
#include "esp_camera.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include "roverMod.h"
#include "pirMod.h"

#define DEEP_SLEEP_DURATION_MILLIS 30 * 1000

// Deep sleep duration (10 seconds in microseconds)
#define DEEP_SLEEP_DURATION_MICROS 1000 * DEEP_SLEEP_DURATION_MILLIS

// Wake duration to check for connections (milliseconds)
#define WAKE_CHECK_DURATION_MILLIS 3 * 1000

static const char *TAG = "sleepMod";

static int shouldSleep = 1;

// Function to enter deep sleep
static void tryToSleep()
{
    if (!shouldSleep || ifAPModeOn() || ifMotionDetected())
    {
        ESP_LOGI(TAG, "Sleep mode disabled, ignoring this cycle");
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

void sleepModOff()
{
    ESP_LOGI(TAG, "Turning sleep OFF");
    shouldSleep = 0;
}

void sleepModOn()
{
    ESP_LOGI(TAG, "Turning sleep ON");
    shouldSleep = 1;
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
