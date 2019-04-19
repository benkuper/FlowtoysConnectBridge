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

  int getButtonStateVal(int id) { return preferences.getInt(String("buttonValue"+String(id)).c_str(),0); }
  void setButtonStateVal(int id, int val) { preferences.putInt(String("buttonValue"+String(id)).c_str(), val); }
  
};

Config * Config::instance = nullptr;
