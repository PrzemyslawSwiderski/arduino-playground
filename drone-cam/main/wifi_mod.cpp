#include "wifi_mod.hpp"
#include "secrets.hpp"
#include "logger.hpp"
#include "wifi_sta.hpp"
#include <memory>

using namespace std::chrono_literals;

namespace wifi
{
  static std::unique_ptr<espp::WifiSta> wifi_sta;
  static espp::Logger logger({.tag = "WIFI", .level = espp::Logger::Verbosity::INFO});

  void start_wifi_task()
  {
    if (wifi_sta)
    {
      logger.warn("WiFi already running");
      return;
    }

    logger.info("Starting WiFi...");

    auto wifi_config = espp::WifiSta::Config{
        .ssid = CONFIG_ESP_WIFI_SSID,
        .password = CONFIG_ESP_WIFI_PASSWORD,
        .phy_rate = WIFI_PHY_RATE_MCS9_LGI,
        .num_connect_retries = 3,
        .on_connected = []()
        { logger.info("WiFi connected"); },
        .on_disconnected = []()
        { logger.warn("WiFi disconnected"); },
        .on_got_ip = [](ip_event_got_ip_t *eventdata)
        { logger.info("got IP: {}.{}.{}.{}", IP2STR(&eventdata->ip_info.ip)); },
        .log_level = espp::Logger::Verbosity::DEBUG};

    wifi_sta = std::make_unique<espp::WifiSta>(wifi_config);

    // Wait for connection (blocking, but quick)
    while (!wifi_sta->is_connected())
    {
      std::this_thread::sleep_for(100ms);
    }

    logger.info("WiFi init complete!");
  }

  void stop_wifi_task()
  {
    if (wifi_sta)
    {
      logger.info("Stopping WiFi...");
      wifi_sta.reset();
      logger.info("WiFi stopped");
    }
    else
    {
      logger.warn("WiFi not running");
    }
  }
}
