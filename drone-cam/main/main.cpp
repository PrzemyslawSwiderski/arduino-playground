#include "bsp/esp-bsp.h"

#include <chrono>
#include "logger.hpp"

#include "music_mod.hpp"
#include "ftp_mod.hpp"
#include "wifi_mod.hpp"
#include "control_mod.hpp"
#include "cam_server.h"

using namespace std::chrono_literals;

extern "C" void app_main()
{ // Keep app_main as C linkage if needed

  espp::Logger logger({.tag = "MAIN", .level = espp::Logger::Verbosity::DEBUG});

  logger.info("Bootup");

  // Init SD Card
  // ESP_ERROR_CHECK(bsp_sdcard_mount());

  music::play();
  // init wifi
  wifi::start_wifi_task();

  // Start FTP server in separate FreeRTOS task
  ftp::start_server_task();
  control::start_control_task();
  cam_server_start();
}
