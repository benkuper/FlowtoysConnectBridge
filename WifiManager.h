#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

#include "Config.h"
#include "SerialManager.h"

class WifiManager
{
public:
  WifiManager(){}
  ~WifiManager(){}

  bool isConnected = false;
  
  void init()
  {
    connect();
  }

  void connect()
  {
    isConnected = false;
    
    String ssid = Config::instance->getWifiSSID();
    String pass = Config::instance->getWifiPassword();
    
    DBG("Connecting to Wifi "+ssid+" with password "+pass+"...");

    if(ssid != "notset" && pass != "notset")
    {
      WiFi.begin(ssid.c_str(), pass.c_str());
    
      int curTry = 0;
      bool connected = true;
      while (WiFi.status() != WL_CONNECTED) {
        if (curTry > 50)
        {
          connected = false;
          break;
        }
        delay(500);
        curTry ++;
      }

      isConnected = connected;
      DBG(isConnected?"WiFi Connected.":"WiFi Connection Error");
    }else
    {
      DBG("Not connecting, ssid or pass not set.");
    }
  }
};

