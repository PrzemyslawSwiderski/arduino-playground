#include <WiFi.h>
#include "secrets.h"
#include "wifiMod.h"
#include "sleepMod.h"
#include "prefsMod.h"
#include "pins.h"
#include "esp_log.h"

static const char *TAG = "wifiMod";

static String wiFiAddr = "";

static String wiFiSSID = "";
static String wiFiPass = "";

static bool isAPMode = true;

void saveCurrentMode()
{
  saveBoolean("isAPMode", isAPMode);
}

void saveWiFiCreds(String wiFiSSID, String wiFiPass)
{
  saveString("wiFiSSID", wiFiSSID);
  saveString("wiFiPass", wiFiPass);
}

void startAsClient()
{
  sleepModOn();
  ESP_LOGI(TAG, "Running in Client mode...");
  ESP_LOGI(TAG, "Connecting to: %s", wiFiSSID);
  WiFi.begin(wiFiSSID, wiFiPass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    ESP_LOGI(TAG, ".");
    if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_NO_SSID_AVAIL)
    {
      ESP_LOGW(TAG, "Can not connect to WiFi with provided credentials");
      return;
    }
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
  ESP_LOGI(TAG, "Device Ready! Use 'http://%s' to connect", wiFiAddr);
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
  isAPMode = readBool("isAPMode");
  wiFiSSID = readString("wiFiSSID");
  wiFiPass = readString("wiFiPass");

  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  runWifiMode();
  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);
}
