#include "webSocketMod.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include "esp_camera.h"
#include <WebSocketsServer.h>
#include <unordered_map>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "webSocketMod";

static WebSocketsServer ws(443);

static QueueHandle_t vidRequestQueue; // Queue for video requests

// Struct for video requests
typedef struct
{
  uint8_t clientId;
} VidRequest;

static void sendFrame(uint8_t clientId)
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(TAG, "Camera capture failed");
    ws.sendTXT(clientId, "Error: Camera capture failed");
    return;
  }
  ws.sendBIN(clientId, fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

static void sendInfo(uint8_t clientId)
{
  size_t totalHeapSize = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  float freeHeapPercent = (freeHeap * 100.0) / totalHeapSize;

  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "Free Heap: %u bytes (%.2f%% of %u total)\n"
           "Clients: %u\n",
           freeHeap, freeHeapPercent, totalHeapSize, ws.connectedClients());
  ESP_LOGI(TAG, "Sending info to client %u:\n%s", clientId, buffer);
  ws.sendTXT(clientId, buffer);
}

// Command map
static const std::unordered_map<DataKey, std::function<void(uint8_t)>, DataKeyHash> commands = {
    {DataKey{(const uint8_t *)"go", 2}, [](uint8_t clientId)
     { roverFwd(); }},
    {DataKey{(const uint8_t *)"stop", 4}, [](uint8_t clientId)
     { roverStop(); }},
    {DataKey{(const uint8_t *)"left", 4}, [](uint8_t clientId)
     { roverLeft(); }},
    {DataKey{(const uint8_t *)"right", 5}, [](uint8_t clientId)
     { roverRight(); }},
    {DataKey{(const uint8_t *)"back", 4}, [](uint8_t clientId)
     { roverBack(); }},
    {DataKey{(const uint8_t *)"ledon", 5}, [](uint8_t clientId)
     { ledOn(); }},
    {DataKey{(const uint8_t *)"ledoff", 6}, [](uint8_t clientId)
     { ledOff(); }},
    {DataKey{(const uint8_t *)"vid", 3}, sendFrame},
    {DataKey{(const uint8_t *)"info", 4}, sendInfo},
    {DataKey{(const uint8_t *)"reset", 5}, [](uint8_t clientId)
     { ESP.restart(); }},
    {DataKey{(const uint8_t *)"wifi-change", 11}, [](uint8_t clientId)
     { changeWifiMode(); }},
};

// WebSocket event handler
void onWsEvent(uint8_t clientId, WStype_t type, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    ESP_LOGI(TAG, "Client disconnected: ID %u", clientId);
    break;
  case WStype_CONNECTED:
    ESP_LOGI(TAG, "Client connected: ID %u", clientId);
    ws.sendTXT(clientId, "Hello from ESP32-CAM!");
    break;
  case WStype_TEXT:
    ESP_LOGD(TAG, "Received cmd: %.*s", (int)len, (char *)data);
    {
      DataKey key{data, len};
      auto it = commands.find(key);
      if (it != commands.end())
      {
        it->second(clientId);
      }
      else
      {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Unrecognized message: %.*s", (int)len, (char *)data);
        ws.sendTXT(clientId, buffer);
      }
    }
    break;
  case WStype_ERROR:
    ESP_LOGI(TAG, "Error with client %u", clientId);
    break;
  default:
    break;
  }
}

// WebSocket task function
static void webSocketTask(void *pvParameters)
{
  while (1)
  {
    ws.loop();                    // Handle WebSocket events
    vTaskDelay(pdMS_TO_TICKS(5)); // Yield to other tasks
  }
}

void setupWebSocketMod()
{
  ws.begin();
  ws.onEvent(onWsEvent);
  ESP_LOGI(TAG, "WebSocket server started on ws://<your-ip>:443/ws");

  // Create WebSocket task
  xTaskCreate(
      webSocketTask,
      "WebSocketTask",
      16384,
      NULL,
      10, // Priority (1-25, higher is more urgent)
      NULL);
}
