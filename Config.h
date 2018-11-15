#pragma once
#include "Preferences.h"

class Config
{
public:
  static Config * instance;
  
  Preferences preferences;
  
  Config()
  {
    instance = this;
  }

  ~Config(){}

  void init()
  {
    preferences.begin("flow", false);
  }

  uint32_t getRFNetworkId() { return preferences.getULong("net", 0); }
  void setRFNetworkId(uint32_t netId) { preferences.putULong("net", netId); }
  
  String getWifiSSID() { return preferences.getString("ssid","notset"); }
  void setWifiSSID(String ssid) { preferences.putString("ssid",ssid); }
  
  String getWifiPassword() { return preferences.getString("pass","notset"); }
  void setWifiPassword(String pass) { preferences.putString("pass", pass); }
};

Config * Config::instance = nullptr;
