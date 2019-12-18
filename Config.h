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
    preferences.begin("bridge", false);
  }

  uint32_t getRFNetworkId(int privateGroupIndex) { return preferences.getULong(String("privateGroup"+String(privateGroupIndex)).c_str(), 0); }
  void setRFNetworkId(int privateGroupIndex, uint32_t groupID) { preferences.putULong(String("privateGroup"+String(privateGroupIndex)).c_str(), groupID); }
  
  String getWifiSSID() { return preferences.getString("ssid","flowspace"); }
  void setWifiSSID(String ssid) { preferences.putString("ssid",ssid); }
  
  String getWifiPassword() { return preferences.getString("pass","flowarts"); }
  void setWifiPassword(String pass) { preferences.putString("pass", pass); }

  int getButtonStateVal(int id) { return preferences.getInt(String("buttonValue"+String(id)).c_str(),0); }
  void setButtonStateVal(int id, int val) { preferences.putInt(String("buttonValue"+String(id)).c_str(), val); }
  
};

Config * Config::instance = nullptr;
