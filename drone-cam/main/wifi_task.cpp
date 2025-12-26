#include <chrono>

#include "secrets.hpp"
#include "logger.hpp"
#include "wifi_sta.hpp"

using namespace std::chrono_literals;

namespace wifi
{

  static TaskHandle_t wifi_task_handle = nullptr;
  static bool running = false;

  void wifi_task(void *pvParameters)
  {
    espp::Logger logger({.tag = "WIFI", .level = espp::Logger::Verbosity::INFO});
    logger.info("Starting WiFi...");
    espp::WifiSta::Config config{.ssid = CONFIG_ESP_WIFI_SSID,         // use whatever was saved to NVS (if any)
                                 .password = CONFIG_ESP_WIFI_PASSWORD, // use whatever was saved to NVS (if any)
                                 .num_connect_retries = 3,
                                 .on_connected = nullptr,
                                 .on_disconnected = nullptr,
                                 .on_got_ip =
                                     [&](ip_event_got_ip_t *eventdata)
                                 {
                                   logger.info("got IP: {}.{}.{}.{}",
                                               IP2STR(&eventdata->ip_info.ip));
                                 },
                                 .log_level = espp::Logger::Verbosity::DEBUG};

    espp::WifiSta wifi_sta(config);
    while (!wifi_sta.is_connected())
    {
      std::this_thread::sleep_for(100ms);
    }

    logger.info("WiFi init complete!");

    // Keep running until task is deleted
    while (running)
    {
      vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
    }

    logger.info("WiFi stopping...");
    wifi_task_handle = nullptr;
    vTaskDelete(nullptr); // Delete self
  }

  void start_wifi_task()
  {
    if (wifi_task_handle == nullptr)
    {
      running = true;
      xTaskCreate(
          wifi_task, // Task function
          "wifi",    // Task name
          8192,      // Stack size (adjust as needed)
          nullptr,   // Parameters
          3,         // Priority (adjust as needed)
          &wifi_task_handle);
    }
  }

  void stop_server_task()
  {
    if (wifi_task_handle != nullptr)
    {
      running = false;
      // Task will delete itself
    }
  }

}
