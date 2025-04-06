#include <WiFi.h>
#include "secrets.h"
#include "wifiMod.h"

String WiFiAddr = "";

static bool isAPMode = false;

namespace
{

  void startAsClient()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    WiFiAddr = WiFi.localIP().toString();
    isAPMode = false;
  }

  void startAsAP()
  {
    WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    WiFiAddr = myIP.toString();
    isAPMode = true;
  }

  void stopWifi()
  {
    WiFi.disconnect(true); // Disconnect and turn off Wi-Fi
    WiFi.mode(WIFI_OFF);   // Ensure Wi-Fi is fully off
    delay(100);            // Give it time to settle
  }

  void printIpInfo()
  {
    Serial.print("Rover Ready! Use 'http://");
    Serial.print(WiFiAddr);
    Serial.println("' to connect");
  }

}

void changeWifiMode()
{
  // Toggle mode
  if (isAPMode)
  {
    Serial.println("Switching to Client mode...");
    stopWifi();      // Disconnect current mode
    startAsClient(); // Start Client mode
  }
  else
  {
    Serial.println("Switching to AP mode...");
    stopWifi();  // Disconnect current mode
    startAsAP(); // Start AP mode
  }
  printIpInfo();
}

void setupWifiMod()
{
  startAsClient();
  printIpInfo();
}
