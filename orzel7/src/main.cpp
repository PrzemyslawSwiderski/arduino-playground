#include <Arduino.h>
#include "esp_camera.h"
#include "pins.h"
#include "cameraMod.h"
#include "roverMod.h"
#include "wifiMod.h"
#include "webSocketMod.h"
#include "pirMod.h"
#include "sleepMod.h"
#include "pcaMod.h"
#include "esp_log.h"

static const char *TAG = "main";

void setup()
{
  Serial.begin(115200);
  delay(100); // Brief delay for Serial to stabilize

  // Check wake-up reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
    {
      ESP_LOGI(TAG, "Woke up from deep sleep");
  }
  else
  {
    ESP_LOGI(TAG, "Booting from power-on or reset");
  }
  setupPcaMod();
  setupRoverMod();
  setupCameraMod();

  setupWifiMod();
  setupWebSocketMod();
  setupSleepMod();
  setupPirMod();
}

void loop()
{
}
