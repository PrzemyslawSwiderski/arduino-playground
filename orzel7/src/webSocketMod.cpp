#include "webSocketMod.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "wifiMod.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include <unordered_map>
#include <functional>

static AsyncWebServer server(443);
static AsyncWebSocket ws("/ws");

static unsigned long lastHealthCheckTime = 0;

const unsigned long HEALTH_CHECK_INTERVAL = 5000; // 5 seconds in milliseconds

static void sendFrame(AsyncWebSocketClient *client)
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }
  client->binary(fb->buf, fb->len);

  // Return frame buffer
  esp_camera_fb_return(fb);
}

static void sendInfo(AsyncWebSocketClient *client)
{
  client->printf("Can write all: %d", ws.availableForWriteAll());
  client->printf("Ping all result: %d", ws.pingAll());
  client->printf("Current connected clients: %u", ws.count());

  size_t totalHeapSize = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  float freeHeapPercent = (freeHeap * 100.0) / totalHeapSize;
  client->printf("Free Heap: %u bytes (%.2f%% of %u total)", freeHeap, freeHeapPercent, totalHeapSize);
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
    Serial.printf("Client connected: ID %lu, IP %s\n", client->id(), client->remoteIP().toString().c_str());
    client->setCloseClientOnQueueFull(false);
    client->text("Hello from ESP32-CAM!");
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("Client disconnected: ID %lu at %lu ms\n", (unsigned long)client->id(), millis());
    client->close();
    break;
  case WS_EVT_ERROR:
    Serial.printf("Error with client %lu: %s\n", (unsigned long)client->id(), (char *)data);
    break;
  case WS_EVT_DATA:
    handleDataEvent(client, arg, data, len);
    break;
  default:
    break;
  }
}

void healthCheck()
{
  ws.cleanupClients();
  Serial.printf("Ping all result: %d\n", ws.pingAll());
  Serial.printf("Current connected clients: %u\n", ws.count());
  size_t totalHeapSize = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  float freeHeapPercent = (freeHeap * 100.0) / totalHeapSize; // Calculate percentage
  Serial.printf("Free Heap: %u bytes (%.2f%% of %u total)\n", freeHeap, freeHeapPercent, totalHeapSize);
}

void setupWebSocketMod()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
  Serial.println("WebSocket server started on ws://<your-ip>:443/ws");
}

void loopWebSocketMod()
{
  runEvery(lastHealthCheckTime, HEALTH_CHECK_INTERVAL, healthCheck);
}
