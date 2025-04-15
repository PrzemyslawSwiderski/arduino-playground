#include <Arduino.h>
#include "esp_camera.h"
#include "pins.h"
#include "cameraMod.h"
#include "roverMod.h"
#include "wifiMod.h"
#include "webSocketMod.h"
#include "pirMod.h"
#include "esp_log.h"

static const char *TAG = "main";

void setup()
{
  Serial.begin(115200);

  setupRoverMod();
  setupCameraMod();

  setupWifiMod();
  setupWebSocketMod();
  setupPirMod();
}

void loop()
{
}
