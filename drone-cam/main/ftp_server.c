#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#define WIFI_SSID "user"
#define WIFI_PASS "pass"
#define MOUNT_POINT "/sdcard"

static const char *TAG = "FTP_SD";

// WiFi event handler
// static void wifi_event_handler(void* arg, esp_event_base_t event_base,
//                                 int32_t event_id, void* event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         ESP_LOGI(TAG, "Disconnected, retrying...");
//         esp_wifi_connect();
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//         ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        
//         // Start FTP server after getting IP
//         ftp_init(MOUNT_POINT);
//         ESP_LOGI(TAG, "FTP Server started on port 21");
//     }
// }

// void init_wifi(void)
// {
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
//                                                 &wifi_event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
//                                                 &wifi_event_handler, NULL));

//     wifi_config_t wifi_config = {};
//     strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
//     strcpy((char*)wifi_config.sta.password, WIFI_PASS);
//     wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI(TAG, "WiFi initialization finished.");
// }

// void init_sdcard(void)
// {
//     esp_err_t ret;
    
//     sdmmc_host_t host = SDMMC_HOST_DEFAULT();
//     sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    
//     esp_vfs_fat_sdmmc_mount_config_t mount_config = {
//         .format_if_mount_failed = false,
//         .max_files = 5,
//         .allocation_unit_size = 16 * 1024
//     };

//     sdmmc_card_t *card;
//     ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, 
//                                    &mount_config, &card);

//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to mount SD card (%s)", esp_err_to_name(ret));
//         return;
//     }
    
//     ESP_LOGI(TAG, "SD card mounted at %s", MOUNT_POINT);
//     sdmmc_card_print_info(stdout, card);
// }

// void app_main(void)
// {
//     // Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // Initialize SD card
//     init_sdcard();

//     // Initialize WiFi
//     init_wifi();
    
//     ESP_LOGI(TAG, "Ready! Connect via FTP to access SD card files");
// }
