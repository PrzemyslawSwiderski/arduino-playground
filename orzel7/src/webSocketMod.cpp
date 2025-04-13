#include "webSocketMod.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include <unordered_map>
#include <functional>

static const char *TAG = "webSockeMod";

static AsyncWebServer server(443);
static AsyncWebSocket ws("/ws");

static unsigned long lastHealthCheckTime = 0;

const unsigned long HEALTH_CHECK_INTERVAL = 5000; // 5 seconds in milliseconds
const unsigned int QUEUE_MARGIN = 5;

static boolean canSend(AsyncWebSocketClient *client)
{
  size_t currentQueueLen = client->queueLen();
  if (currentQueueLen < WS_MAX_QUEUED_MESSAGES - QUEUE_MARGIN)
  {
    return true;
  }
  else
  {
    ESP_LOGE(TAG, "Can not send to client, ignoring the message");
    return false;
  }
}

static void sendFrame(AsyncWebSocketClient *client)
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGE(TAG, "Camera capture failed");
    return;
  }
  if (canSend(client))
  {
    client->binary(fb->buf, fb->len);
  }

  // Return frame buffer
  esp_camera_fb_return(fb);
}

static void sendInfo(AsyncWebSocketClient *client)
{
  size_t queueLen = client->queueLen();
  size_t totalHeapSize = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  float freeHeapPercent = (freeHeap * 100.0) / totalHeapSize;

  char buffer[256]; // Adjust size if needed
  snprintf(buffer, sizeof(buffer),
           "Queue LEN: %u\n"
           "Can write all: %d\n"
           "Ping all result: %d\n"
           "Current connected clients: %u\n"
           "Free Heap: %u bytes (%.2f%% of %u total)\n",
           queueLen,
           ws.availableForWriteAll(),
           ws.pingAll(),
           ws.count(),
           freeHeap, freeHeapPercent, totalHeapSize);
  ESP_LOGI(TAG, "Sending info to client %u:\n%s", client->id(), buffer);
  if (canSend(client))
  {
    client->text(buffer);
  }
}

// Command map defined outside the function (file-scoped static)
static const std::unordered_map<DataKey, std::function<void(AsyncWebSocketClient *)>, DataKeyHash> commands = {
    {DataKey{(const uint8_t *)"go", 2}, [](AsyncWebSocketClient *client)
     { roverFwd(); }},
    {DataKey{(const uint8_t *)"stop", 4}, [](AsyncWebSocketClient *client)
     { roverStop(); }},
    {DataKey{(const uint8_t *)"left", 4}, [](AsyncWebSocketClient *client)
     { roverLeft(); }},
    {DataKey{(const uint8_t *)"right", 5}, [](AsyncWebSocketClient *client)
     { roverRight(); }},
    {DataKey{(const uint8_t *)"back", 4}, [](AsyncWebSocketClient *client)
     { roverBack(); }},
    {DataKey{(const uint8_t *)"ledon", 5}, [](AsyncWebSocketClient *client)
     { ledOn(); }},
    {DataKey{(const uint8_t *)"ledoff", 6}, [](AsyncWebSocketClient *client)
     { ledOff(); }},
    {DataKey{(const uint8_t *)"vid", 3}, sendFrame},
    {DataKey{(const uint8_t *)"info", 4}, sendInfo},
    {DataKey{(const uint8_t *)"reset", 5}, [](AsyncWebSocketClient *client)
     { ESP.restart(); }},
    {DataKey{(const uint8_t *)"wifi-change", 11}, [](AsyncWebSocketClient *client)
     { changeWifiMode(); }},
};

// WebSocket event handler
static void handleDataEvent(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  ESP_LOGI(TAG, "Received cmd: %.*s", (int)len, (char *)data);
  DataKey key{data, len};
  auto it = commands.find(key);
  if (it != commands.end())
  {
    it->second(client);
    return;
  }

  // Handle unrecognized message
  client->printf("Unrecognized message: %.*s", (int)len, (char *)data);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    ESP_LOGI(TAG, "Client connected: ID %lu, IP %s\n", client->id(), client->remoteIP().toString().c_str());
    client->setCloseClientOnQueueFull(false);
    client->text("Hello from ESP32-CAM!");
    break;
  case WS_EVT_DISCONNECT:
    ESP_LOGI(TAG, "Client disconnected: ID %u at %lu ms\n", client->id(), millis());
    client->close();
    break;
  case WS_EVT_ERROR:
    ESP_LOGI(TAG, "Error with client %u: %s\n", client->id(), data);
    break;
  case WS_EVT_DATA:
    handleDataEvent(client, arg, data, len);
    break;
  default:
    break;
  }
}

void setupWebSocketMod()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
  ESP_LOGI(TAG, "WebSocket server started on ws://<your-ip>:443/ws");
}
