#include "webSocketMod.h"
#include "cameraMod.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include "sleepMod.h"
#include "pirMod.h"
#include "esp_camera.h"
#include <WebSocketsServer.h>
#include <unordered_map>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "webSocketMod";

static WebSocketsServer ws(443);

static unsigned long lastHealthCheck = 0;

static void sendFrame(uint8_t clientId)
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(TAG, "Camera capture failed");
    ws.sendTXT(clientId, "Error: Camera capture failed, resetting camera...");
    resetCameraMod();
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

  size_t totalPsramSize = ESP.getPsramSize();
  size_t freePsram = ESP.getFreePsram();
  float freePsramPercent = (freePsram * 100.0) / totalPsramSize;

  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "Free Heap: %u bytes (%.2f%% of %u total)\n"
           "Free PSRAM: %u bytes (%.2f%% of %u total)\n"
           "Clients: %u\n",
           freeHeap, freeHeapPercent, totalHeapSize,
           freePsram, freePsramPercent, totalPsramSize,
           ws.connectedClients());
  ESP_LOGI(TAG, "Sending info to client %u:\n%s", clientId, buffer);
  ws.sendTXT(clientId, buffer);
}

// Command map
static const std::unordered_map<DataKey, std::function<void(uint8_t)>, DataKeyHash> simpleCommands = {
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
    {DataKey{(const uint8_t *)"reset-cam", 9}, [](uint8_t clientId)
     { resetCameraMod(); }},
    {DataKey{(const uint8_t *)"wifi-change", 11}, [](uint8_t clientId)
     { switchWifiMode(); }},
    {DataKey{(const uint8_t *)"piron", 5}, [](uint8_t clientId)
     { switchPirOn(); }},
    {DataKey{(const uint8_t *)"piroff", 6}, [](uint8_t clientId)
     { switchPirOff(); }},
};

// Command map for integer value commands
static const std::unordered_map<DataKey, std::function<void(int)>, DataKeyHash> intValueCommands = {
    {DataKey{(const uint8_t *)"quality", 7}, setQuality},
    {DataKey{(const uint8_t *)"brightness", 10}, setBrightness},
    {DataKey{(const uint8_t *)"size", 4}, setSize},
    {DataKey{(const uint8_t *)"contrast", 8}, setContrast},
};

// Command map for string value commands
static const std::unordered_map<DataKey, std::function<void(const String &)>, DataKeyHash> stringValueCommands = {
    {DataKey{(const uint8_t *)"wifi-ssid", 9}, setWifiSsid},
    {DataKey{(const uint8_t *)"wifi-pass", 9}, setWifiPassword},
};

// Helper: Send error message to client
static void sendError(uint8_t clientId, const char *format, ...)
{
  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  ws.sendTXT(clientId, buffer);
}

// Helper: Parse and validate integer value from string
static bool parseValue(const char *valueStr, size_t valueLen, int &result)
{
  char valueBuf[16];
  if (valueLen >= sizeof(valueBuf))
  {
    return false; // Value too long
  }
  strncpy(valueBuf, valueStr, valueLen);
  valueBuf[valueLen] = '\0';
  char *endptr;
  long value = strtol(valueBuf, &endptr, 10);
  if (endptr == valueBuf || *endptr != '\0')
  {
    return false; // Invalid number
  }
  result = (int)value;
  return true;
}

// Helper: Handle key-value command
static void handleValueCommand(uint8_t clientId, uint8_t *data, size_t len, char *colon)
{
  size_t keyLen = colon - (char *)data;
  DataKey key{data, keyLen};

  // Check integer command map
  auto intEntry = intValueCommands.find(key);
  if (intEntry != intValueCommands.end())
  {
    // Parse value after colon
    char *valueStr = colon + 1;
    size_t valueLen = len - keyLen - 1;
    int value;
    if (!parseValue(valueStr, valueLen, value))
    {
      sendError(clientId, valueLen >= 16 ? "Error: Value too long" : "Error: Invalid number");
      return;
    }
    intEntry->second(value);
    ESP_LOGI(TAG, "Processed int command: %.*s=%d", (int)keyLen, (char *)data, value);
    return;
  }

  // Check String command map
  auto strEntry = stringValueCommands.find(key);
  if (strEntry != stringValueCommands.end())
  {
    // Extract string value after colon
    char *valueStr = colon + 1;
    size_t valueLen = len - keyLen - 1;
    // Create String from value
    String value = String(valueStr, valueLen);
    strEntry->second(value);
    ESP_LOGI(TAG, "Processed String command: %.*s=%s", (int)keyLen, (char *)data, value.c_str());
    return;
  }

  // If no command found
  sendError(clientId, "Unrecognized command: %.*s", (int)keyLen, (char *)data);
}

// Helper: Handle simple command
static void handleSimpleCommand(uint8_t clientId, uint8_t *data, size_t len)
{
  DataKey key{data, len};
  auto entry = simpleCommands.find(key);
  if (entry != simpleCommands.end())
  {
    entry->second(clientId);
  }
  else
  {
    sendError(clientId, "Unrecognized message: %.*s", (int)len, (char *)data);
  }
}

// WebSocket event handler
void onWsEvent(uint8_t clientId, WStype_t type, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    ESP_LOGI(TAG, "Client disconnected: ID %u", clientId);
    sleepModOn();
    break;
  case WStype_CONNECTED:
    sleepModOff();
    ESP_LOGI(TAG, "Client connected: ID %u", clientId);
    ws.sendTXT(clientId, "Hello from ESP32-CAM!");
    break;
  case WStype_TEXT:
    ESP_LOGD(TAG, "Received cmd: %.*s", (int)len, (char *)data);
    {
      char *colon = (char *)memchr(data, ':', len);
      if (colon)
      {
        handleValueCommand(clientId, data, len, colon);
      }
      else
      {
        handleSimpleCommand(clientId, data, len);
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

static void healthCheck()
{
  ESP_LOGI(TAG, "HEALTH CHECK");
  boolean res = ws.broadcastPing();
  if (!res)
  {
    ESP_LOGI(TAG, "HEALTH CHECK FAILED");
    ws.disconnect();
  }
  else
  {
    int clients = ws.connectedClients(true);
    ESP_LOGI(TAG, "CLIENTS: %d", clients);
    ESP_LOGI(TAG, "HEALTH CHECK SUCCESS");
    if (clients)
    {
      sleepModOff();
    }
    else
    {
      sleepModOn();
    }
  }
}

// WebSocket task function
static void webSocketTask(void *pvParameters)
{
  while (1)
  {
    ws.loop();                    // Handle WebSocket events
    vTaskDelay(pdMS_TO_TICKS(5)); // Yield to other tasks
    runEvery(lastHealthCheck, 5000, healthCheck);
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
