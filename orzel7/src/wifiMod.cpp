#include <WiFi.h>
#include "secrets.h"
#include "wifiMod.h"
#include "prefsMod.h"
#include "utilsMod.h"
#include "pins.h"
#include "esp_log.h"

static const char *TAG = "wifiMod";
static const char *IS_AP_MODE_KEY = "isAPMode";
static const char *WIFI_SSID_KEY = "wifiSsid";
static const char *WIFI_PASS_KEY = "wifiPass";
static const unsigned long CONNECT_TIMEOUT_MS = 2000;

static String wiFiAddr = "";

static String wifiSsid = "";
static String wifiPass = "";

static bool isAPMode = true;

bool ifAPModeOn()
{
  return isAPMode;
}

void saveCurrentMode()
{
  saveBoolean(IS_AP_MODE_KEY, isAPMode);
}

void setWifiSsid(String wifiSsid)
{
  saveString(WIFI_SSID_KEY, wifiSsid);
}

void setWifiPassword(String wifiPass)
{
  saveString(WIFI_PASS_KEY, wifiPass);
}

void startAsAP()
{
  ESP_LOGI(TAG, "Running in AP mode...");
  WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  ESP_LOGI(TAG, "AP IP address: %s", myIP);
  wiFiAddr = myIP.toString();

  saveCurrentMode();
}

void stopWifi()
{
  ESP_LOGI(TAG, "Stopping the WiFi");
  WiFi.disconnect(true); // Disconnect and turn off Wi-Fi
  WiFi.mode(WIFI_OFF);   // Ensure Wi-Fi is fully off
  delay(100);            // Give it time to settle
}

bool isConnectionFailed(unsigned long startTime)
{
  unsigned long currentTime = millis();
  return WiFi.status() == WL_CONNECT_FAILED ||
         WiFi.status() == WL_NO_SSID_AVAIL ||
         isTimeAfter(currentTime, startTime, CONNECT_TIMEOUT_MS);
}

void startAsClient()
{
  ESP_LOGI(TAG, "Running in Client mode...");
  ESP_LOGI(TAG, "Connecting to: %s", wifiSsid.c_str());
  WiFi.begin(wifiSsid, wifiPass);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    ESP_LOGI(TAG, ".");
    if (isConnectionFailed(startTime))
    {
      ESP_LOGW(TAG, "Can not connect to WiFi with provided credentials");
      stopWifi();
      startAsAP();
      return;
    }
  }
  ESP_LOGI(TAG, "WiFi connected");
  wiFiAddr = WiFi.localIP().toString();

  saveCurrentMode();
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
  isAPMode = readBool(IS_AP_MODE_KEY);
  wifiSsid = readString(WIFI_SSID_KEY);
  wifiPass = readString(WIFI_PASS_KEY);

  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  runWifiMode();
  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);
}
