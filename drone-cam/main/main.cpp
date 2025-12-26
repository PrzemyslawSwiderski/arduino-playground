#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "music.h"
#include "wifi.h"

#include <chrono>
#include "ftp_server.hpp"

#include "ftp_task.hpp"
#include "wifi_task.hpp"

using namespace std::chrono_literals;

extern "C" void app_main()
{ // Keep app_main as C linkage if needed

  espp::Logger logger({.tag = "MAIN", .level = espp::Logger::Verbosity::DEBUG});

  logger.info("Bootup");

  // Init SD Card
  ESP_ERROR_CHECK(bsp_sdcard_mount());

  // init_wifi();
  wifi::start_wifi_task();
  play_music();

  // Start FTP server in separate FreeRTOS task
  ftp::start_server_task();
}
