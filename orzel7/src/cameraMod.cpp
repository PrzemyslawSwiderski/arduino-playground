#include "Arduino.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "pins.h"
#include "cameraMod.h"

static const char *TAG = "cameraMod";

void setupCameraMod()
{

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 4;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // drop down frame size for higher initial frame rate
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);
}

void setQuality(int value)
{
  if (value < 0 || value > 64)
  {
    ESP_LOGW(TAG, "Error: Quality must be 0-63");
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  ESP_LOGI(TAG, "Changing quality to: %d", value);
  s->set_quality(s, value);
}

void setBrightness(int value)
{
  if (value < -2 || value > 2)
  {
    ESP_LOGW(TAG, "Error: Brightness must be -2 to 2");
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  ESP_LOGI(TAG, "Changing brightness to: %d", value);
  s->set_brightness(s, value);
}

static bool intToFrameSize(int value, framesize_t &frameSize)
{
  switch (value)
  {
  case 0:
    frameSize = FRAMESIZE_96X96;
    return true;
  case 1:
    frameSize = FRAMESIZE_QQVGA;
    return true;
  case 2:
    frameSize = FRAMESIZE_128X128;
    return true;
  case 3:
    frameSize = FRAMESIZE_QCIF;
    return true;
  case 4:
    frameSize = FRAMESIZE_HQVGA;
    return true;
  case 5:
    frameSize = FRAMESIZE_240X240;
    return true;
  case 6:
    frameSize = FRAMESIZE_QVGA;
    return true;
  case 7:
    frameSize = FRAMESIZE_320X320;
    return true;
  case 8:
    frameSize = FRAMESIZE_CIF;
    return true;
  case 9:
    frameSize = FRAMESIZE_HVGA;
    return true;
  case 10:
    frameSize = FRAMESIZE_VGA;
    return true;
  case 11:
    frameSize = FRAMESIZE_SVGA;
    return true;
  case 12:
    frameSize = FRAMESIZE_XGA;
    return true;
  case 13:
    frameSize = FRAMESIZE_HD;
    return true;
  default:
    return false; // Invalid framesize
  }
}

void setSize(int value)
{
  framesize_t frameSize;
  if (!intToFrameSize(value, frameSize))
  {
    ESP_LOGW(TAG, "Error: Framesize must be 0-13");
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  ESP_LOGI(TAG, "Changing Framesize to: %d", value);
  s->set_framesize(s, frameSize);
}

void setContrast(int value)
{
  if (value < -2 || value > 2)
  {
    ESP_LOGW(TAG, "Error: Contrast must be -2 to 2");
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  ESP_LOGI(TAG, "Changing contrast to: %d", value);
  s->set_contrast(s, value);
}
