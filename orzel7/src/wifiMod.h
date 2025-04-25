#pragma once
#include <WString.h>

bool ifAPModeOn();
void setWifiSsid(String wifiSsid);
void setWifiPassword(String wifiPass);
void switchWifiMode();
void setupWifiMod();
void saveWiFiCreds(String wiFiSSID, String wiFiPass);
