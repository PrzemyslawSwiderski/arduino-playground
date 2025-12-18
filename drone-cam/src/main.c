#include "esp_log.h"
#include "modules/sound.h"
#include "modules/wifi.h"

static const char *TAG = "MAIN";

void app_main(void)
{
  ESP_LOGI(TAG, "START");
  // init_sound();
  // play_sound();
  init_wifi();
}
