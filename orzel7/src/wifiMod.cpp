#include <WiFi.h>
#include <Preferences.h>
#include "secrets.h"
#include "wifiMod.h"
#include "sleepMod.h"
#include "pins.h"
#include "esp_log.h"

static const char *TAG = "wifiMod";

Preferences preferences;

static String wiFiAddr = "";

static bool isAPMode = true;

void saveCurrentMode()
{
  preferences.begin("wifi_mode", false);
  preferences.putBool("isAPMode", isAPMode);
  preferences.end();
}

void startAsClient()
{
  sleepModOn();
  ESP_LOGI(TAG, "Running in Client mode...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    ESP_LOGI(TAG, ".");
  }
  ESP_LOGI(TAG, "WiFi connected");
  wiFiAddr = WiFi.localIP().toString();

  saveCurrentMode();
}

void startAsAP()
{
  sleepModOff();
  ESP_LOGI(TAG, "Running in AP mode...");
  WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  ESP_LOGI(TAG, "AP IP address: %s", myIP);
  wiFiAddr = myIP.toString();

  saveCurrentMode();
}

void stopWifi()
{
  WiFi.disconnect(true); // Disconnect and turn off Wi-Fi
  WiFi.mode(WIFI_OFF);   // Ensure Wi-Fi is fully off
  delay(100);            // Give it time to settle
}

void printIpInfo()
{
  ESP_LOGI(TAG, "Rover Ready! Use 'http://%s' to connect", wiFiAddr);
}

void runWifiMode()
{
  // Toggle mode
  if (isAPMode)
  {
    ESP_LOGI(TAG, "Running in Client mode...");
    stopWifi();      // Disconnect current mode
    startAsClient(); // Start Client mode
  }
  else
  {
    ESP_LOGI(TAG, "Running in AP mode...");
    stopWifi();  // Disconnect current mode
    startAsAP(); // Start AP mode
  }
  printIpInfo();
}

void switchWifiMode()
{
  ESP_LOGI(TAG, "Toggling WIFI mode");
  if (isAPMode)
  {
    isAPMode = false;
  }
  else
  {
    isAPMode = true;
  }
  saveCurrentMode();
  runWifiMode();
}

void setupWifiMod()
{
  // Load isAPMode from Preferences
  preferences.begin("wifi_mode", true);             // Read-only mode
  isAPMode = preferences.getBool("isAPMode", true); // Default to true (AP mode)
  preferences.end();

  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  runWifiMode();
  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);
}
