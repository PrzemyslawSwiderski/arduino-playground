#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "music.h"
#include "wifi.h"

static const char *TAG = "MAIN";

void app_main(void)
{
  ESP_LOGI(TAG, "START");
  ESP_ERROR_CHECK(bsp_spiffs_mount());

  init_wifi();
  play_music();
}
