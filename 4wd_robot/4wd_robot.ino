#include "esp_camera.h"
#include "pins.h"
#include "camera.h"
#include "robot.h"
#include "wifi_conf.h"
#include <FS.h>
#include <SPIFFS.h>

extern void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  File file = SPIFFS.open("/cert.pem", "r");
  if (!file) {
    Serial.println("Failed to open certificate");
    return;
  }

  String cert = file.readString();
  Serial.println(cert)

  pinMode(LED_LIGHT_GPIO_NUM, OUTPUT);
  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  setup_robot();
  setup_camera();

  init_wifi();
  startCameraServer();

  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);
}

void loop() {
}
