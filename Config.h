#pragma once
#include "Preferences.h"

class Config
{
public:
  static Config * instance;

  enum WifiBLEMode { WIFI_ONLY = 0, BLE_ONLY = 1, BOTH = 2 };
  
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

  int getNumPrivateGroups() { return preferences.getInt("numGroups", 0); }
  void setNumPrivateGroups(int num) { preferences.putInt("numGroups", num); }

  String getDeviceName() { return preferences.getString("deviceName", String((int)ESP.getEfuseMac()).substring(0,10)); }
  void setDeviceName(String deviceName) { preferences.putString("deviceName", deviceName.substring(0,10)); }

  bool getWifiMode() { int mode = preferences.getInt("wifiBLEMode",2); return mode == WIFI_ONLY || mode == BOTH; } //0 is wifi only, 1 is BLE only, 2 is both
  bool getBLEMode() { int mode = preferences.getInt("wifiBLEMode",2); return mode == BLE_ONLY || mode == BOTH; }
  void setWifiBLEMode(int mode) { preferences.putInt("wifiBLEMode", mode); }
 
  String getWifiSSID() { return preferences.getString("ssid","flowspace"); }
  void setWifiSSID(String ssid) { preferences.putString("ssid",ssid); }
  
  String getWifiPassword() { return preferences.getString("pass","flowarts"); }
  void setWifiPassword(String pass) { preferences.putString("pass", pass); }

  int getButtonStateVal(int id) { return preferences.getInt(String("buttonValue"+String(id)).c_str(),0); }
  void setButtonStateVal(int id, int val) { preferences.putInt(String("buttonValue"+String(id)).c_str(), val); }
};

Config * Config::instance = nullptr;
