// #include "bsp/esp-bsp.h"

#include <chrono>
#include "logger.hpp"

#include "music_mod.h"
#include "ftp_mod.hpp"
#include "wifi_mod.hpp"
// #include "control_mod.hpp"
// #include "cam_server.h"
#include "video_mod.h"

#include "esp_board_manager.h"
#include "esp_gmf_app_cli.h"
#include "esp_gmf_app_setup_peripheral.h"
#include "esp_gmf_app_sys.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_codec_dev.h"

using namespace std::chrono_literals;

extern "C" void app_main()
{ // Keep app_main as C linkage if needed

  espp::Logger logger({.tag = "MAIN", .level = espp::Logger::Verbosity::DEBUG});

  logger.info("Bootup");

  // void *sdcard_handle = NULL;
  // esp_gmf_app_setup_sdcard(&sdcard_handle);
  esp_board_manager_init();

  esp_gmf_app_cli_init("drone-cam> ", NULL);
  play_music();

  // monitor tasks periodicaly
  // esp_gmf_app_sys_monitor_start();

  // music::play();
  // init wifi
  wifi::start_wifi_task();

  // // Start FTP server in separate FreeRTOS task
  ftp::start_server_task();
  // control::start_control_task();
  // // cam_server_start();
  // run_video_mod();

  // esp_gmf_app_sys_monitor_stop();
}
