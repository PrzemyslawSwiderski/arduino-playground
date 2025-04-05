#include "webSocketMod.h"
#include "roverMod.h"
#include "utilsMod.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"

static AsyncWebServer server(443);
static AsyncWebSocket ws("/ws");

static unsigned long lastFrameTime = 0;
static unsigned long lastCleanupTime = 0;
static unsigned long lastHeartbeatTime = 0;

const unsigned long FRAME_INTERVAL = 50;     // 50 ms = ~20 FPS
const unsigned long CLEANUP_INTERVAL = 5000;  // 5 seconds in milliseconds
const unsigned long HEARTBEAT_INTERVAL = 5000;

static void handleDataEvent(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->opcode == WS_TEXT) {
    String message = String((char *)data, len);
    Serial.println("Received: " + message);

    if (message == "go") {
      roverFwd();
    } else if (message == "stop") {
      roverStop();
    } else if (message == "left") {
      roverLeft();
    } else if (message == "right") {
      roverRight();
    } else if (message == "back") {
      roverBack();
    } else if (message == "ledon") {
      ledOn();
    } else if (message == "ledoff") {
      ledOff();
    } else {
      client->text("Unrecognized message: " + message);
    }
  }
}

static void sendHeartbeat() {
  ws.textAll("{\"type\":\"heartbeat\"}");  // Simple keep-alive message
  Serial.println("Sent heartbeat");
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Client connected: ID %lu, IP %s\n", client->id(), client->remoteIP().toString().c_str());
      client->text("Hello from ESP32-CAM!");
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Client disconnected: ID %lu at %lu ms\n", (unsigned long)client->id(), millis());
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

void streamVideo() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // Send frame to all connected WebSocket clients as binary data
  ws.binaryAll(fb->buf, fb->len);

  // Return frame buffer
  esp_camera_fb_return(fb);
}

void printUsageInfo() {
  size_t totalHeapSize = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  float freeHeapPercent = (freeHeap * 100.0) / totalHeapSize;  // Calculate percentage
  Serial.printf("Free Heap: %u bytes (%.2f%% of %u total)\n", freeHeap, freeHeapPercent, totalHeapSize);
}

void cleanupClients() {
  ws.cleanupClients();
  Serial.println("Cleaned up WebSocket clients");
  Serial.printf("Current connected clients: %u\n", ws.count());
  printUsageInfo();
}

void setupWebSocketMod() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
  Serial.println("Secure WebSocket server started on ws://<your-ip>:443/ws");
}

void loopWebSocketMod() {
  runEvery(lastFrameTime, FRAME_INTERVAL, streamVideo);
  runEvery(lastCleanupTime, CLEANUP_INTERVAL, cleanupClients);
  runEvery(lastHeartbeatTime, HEARTBEAT_INTERVAL, sendHeartbeat);
}
