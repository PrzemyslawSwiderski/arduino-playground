#include "ftp_task.hpp"
#include "ftp_server.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace ftp
{
  static TaskHandle_t server_task_handle = nullptr;
  static bool running = false;

  void server_task(void *pvParameters)
  {
    espp::Logger logger({.tag = "FTP", .level = espp::Logger::Verbosity::DEBUG});

    logger.info("Initializing FTP Server...");
    espp::FtpServer ftp_server("192.168.33.27", 21,
                               espp::FileSystem::get().get_root_path());
    ftp_server.start();
    logger.info("FTP Server started");

    // Keep running until task is deleted
    while (running)
    {
      vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
    }

    logger.info("FTP Server stopping...");
    server_task_handle = nullptr;
    vTaskDelete(nullptr); // Delete self
  }

  void start_server_task()
  {
    if (server_task_handle == nullptr)
    {
      running = true;
      xTaskCreate(
          server_task,        // Task function
          "ftp_server",       // Task name
          8192,               // Stack size (adjust as needed)
          nullptr,            // Parameters
          5,                  // Priority (adjust as needed)
          &server_task_handle // Task handle
      );
    }
  }

  void stop_server_task()
  {
    if (server_task_handle != nullptr)
    {
      running = false;
      // Task will delete itself
    }
  }
}
