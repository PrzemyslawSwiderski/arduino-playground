#include "secrets.h"
#include <WiFi.h>
#include "wifi_conf.h"

String WiFiAddr = "";

static bool isAPMode = false;

namespace {

void start_as_client() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  WiFiAddr = WiFi.localIP().toString();
  isAPMode = false;
}

void start_as_ap() {
  WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  WiFiAddr = myIP.toString();
  isAPMode = true;
}

void stop_wifi() {
  WiFi.disconnect(true);  // Disconnect and turn off Wi-Fi
  WiFi.mode(WIFI_OFF);    // Ensure Wi-Fi is fully off
  delay(100);             // Give it time to settle
}

void print_ip_info() {
  Serial.print("Robot Ready! Use 'http://");
  Serial.print(WiFiAddr);
  Serial.println("' to connect");
}

}

void change_wifi_mode() {
  // Toggle mode
  if (isAPMode) {
    Serial.println("Switching to Client mode...");
    stop_wifi();        // Disconnect current mode
    start_as_client();  // Start Client mode
  } else {
    Serial.println("Switching to AP mode...");
    stop_wifi();    // Disconnect current mode
    start_as_ap();  // Start AP mode
  }
  print_ip_info();
}

void init_wifi() {
  start_as_client();
  print_ip_info();
}
