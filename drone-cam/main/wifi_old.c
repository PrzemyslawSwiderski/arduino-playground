#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_hosted.h"
#include "secrets.h"

static const char *TAG = "WIFI";

// WiFi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
    ESP_LOGW(TAG, "Disconnected from SSID: %s, reason: %d", event->ssid, event->reason);

    // Log common disconnect reasons
    switch (event->reason)
    {
    case WIFI_REASON_AUTH_EXPIRE:
    case WIFI_REASON_AUTH_FAIL:
      ESP_LOGE(TAG, "Authentication failed! Check password.");
      break;
    case WIFI_REASON_NO_AP_FOUND:
      ESP_LOGE(TAG, "AP not found! Check SSID.");
      break;
    case WIFI_REASON_ASSOC_LEAVE:
      ESP_LOGW(TAG, "Disconnected by AP");
      break;
    default:
      ESP_LOGW(TAG, "Other reason");
    }

    ESP_LOGI(TAG, "Disconnected, retrying...");
    esp_wifi_connect();
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
    ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&event->ip_info.gw));
  }
}

// Event handler for WiFi scan
static void wifi_scan_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
  {
    ESP_LOGI(TAG, "Scan completed!");

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    ESP_LOGI(TAG, "Found %d access points:", ap_count);

    if (ap_count > 0)
    {
      wifi_ap_record_t *ap_list = malloc(sizeof(wifi_ap_record_t) * ap_count);
      if (ap_list == NULL)
      {
        ESP_LOGE(TAG, "Failed to allocate memory for AP list");
        return;
      }

      esp_wifi_scan_get_ap_records(&ap_count, ap_list);

      ESP_LOGI(TAG, "");
      ESP_LOGI(TAG, "%-32s %-17s %4s %4s %s", "SSID", "BSSID", "RSSI", "Chan", "Auth");
      ESP_LOGI(TAG, "--------------------------------------------------------------------------------");

      for (int i = 0; i < ap_count; i++)
      {
        const char *auth_mode;
        switch (ap_list[i].authmode)
        {
        case WIFI_AUTH_OPEN:
          auth_mode = "OPEN";
          break;
        case WIFI_AUTH_WEP:
          auth_mode = "WEP";
          break;
        case WIFI_AUTH_WPA_PSK:
          auth_mode = "WPA-PSK";
          break;
        case WIFI_AUTH_WPA2_PSK:
          auth_mode = "WPA2-PSK";
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          auth_mode = "WPA/WPA2-PSK";
          break;
        case WIFI_AUTH_WPA3_PSK:
          auth_mode = "WPA3-PSK";
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          auth_mode = "WPA2/WPA3-PSK";
          break;
        default:
          auth_mode = "UNKNOWN";
        }

        ESP_LOGI(TAG, "%-32s %02x:%02x:%02x:%02x:%02x:%02x %4d %4d %s",
                 ap_list[i].ssid,
                 ap_list[i].bssid[0], ap_list[i].bssid[1],
                 ap_list[i].bssid[2], ap_list[i].bssid[3],
                 ap_list[i].bssid[4], ap_list[i].bssid[5],
                 ap_list[i].rssi,
                 ap_list[i].primary,
                 auth_mode);
      }

      free(ap_list);
    }
  }
}

void diagnose_wifi_module()
{
    ESP_LOGI(TAG, "=== WiFi Module Diagnostics ===");
    
    // Try to get WiFi MAC address
    uint8_t mac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, mac);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "✓ WiFi module responding");
        ESP_LOGI(TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        ESP_LOGE(TAG, "✗ WiFi module NOT responding: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Check: SPI/SDIO connection, C6 power, firmware");
    }
    
    // Check WiFi driver state
    wifi_mode_t mode;
    ret = esp_wifi_get_mode(&mode);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "✓ WiFi driver initialized, mode: %d", mode);
    } else {
        ESP_LOGE(TAG, "✗ WiFi driver issue: %s", esp_err_to_name(ret));
    }
}

void init_wifi()
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  ESP_ERROR_CHECK(esp_hosted_init());
  
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
  //                                            &wifi_event_handler, NULL));
  // ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
  //                                            &wifi_event_handler, NULL));

  // wifi_config_t wifi_config = {
  //     .sta = {
  //         .ssid = CONFIG_WIFI_SSID,
  //         .password = CONFIG_WIFI_PASS,
  //         /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
  //          * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
  //          * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
  //          * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
  //          */
  //         .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK},
  // };
  // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  // ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  // ESP_ERROR_CHECK(esp_wifi_start());

  // Register event handler
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE,
                                             &wifi_scan_event_handler, NULL));

  // Set WiFi to station mode
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  
  diagnose_wifi_module();

  // Small delay to let WiFi fully initialize
  vTaskDelay(pdMS_TO_TICKS(500));

  ESP_LOGI(TAG, "Starting WiFi scan...");

  // Configure scan parameters (optional)
  wifi_scan_config_t scan_config = {
      .ssid = NULL,        // Scan all SSIDs
      .bssid = NULL,       // Scan all BSSIDs
      .channel = 0,        // Scan all channels
      .show_hidden = true, // Show hidden networks
      .scan_type = WIFI_SCAN_TYPE_ACTIVE,
      .scan_time = {
          .active = {
              .min = 100, // Minimum active scan time per channel (ms)
              .max = 300  // Maximum active scan time per channel (ms)
          }}};

  // Start scan (async - results come via event handler)
  esp_wifi_scan_start(&scan_config, true);

  ESP_LOGI(TAG, "WiFi initialization finished.");
}
