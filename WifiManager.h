#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

#include "Config.h"
#include "SerialManager.h"

#define CONNECT_TIMEOUT 5000
#define CONNECT_TRYTIME 500

class WifiManager
{
public:
  WifiManager(){  
      setCallbackConnectionUpdate(&WifiManager::connectionUpdateDefaultCallback);
  }
  
  ~WifiManager(){}

  bool isLocal = false;
  bool isConnected = false;
  bool isConnecting = false;
  
  String ssid;
  String pass;
  
  long timeAtStartConnect;
  long timeAtLastConnect;

  bool isActivated;
  
  void init()
  {
    isActivated = Config::instance->getWifiMode();

    if(isConnected)
    {
      DBG("Disconnecting first...");
      WiFi.disconnect();
      delay(100);
    }
    
    if(!isActivated)
    {
      DBG("Wifi is not activated, not initializing");
      return;
    }

    ssid = Config::instance->getWifiSSID();
    pass = Config::instance->getWifiPassword();

    
    DBG("Connecting to Wifi "+ssid+" with password "+pass+"...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    WiFi.setSleep(false);

    
    timeAtStartConnect = millis();
    timeAtLastConnect = millis();
    
    isLocal = false;
    isConnecting = true;
    setConnected(false);
    
    digitalWrite(13, HIGH);
  }

  void update()
  {
    if(!isActivated) return;
    if(isLocal || isConnected) return;

    if(millis() > timeAtLastConnect + CONNECT_TRYTIME)
    {      
      if(WiFi.status() == WL_CONNECTED)
      {  
         digitalWrite(13, LOW);

         DBG("WiFi Connected, local IP : "+String(WiFi.localIP()[0])+
    "."+String(WiFi.localIP()[1])+
    "."+String(WiFi.localIP()[2])+
    "."+String(WiFi.localIP()[3]));

        isLocal = true;
        setConnected(true);
    
         return;
      }
      timeAtLastConnect = millis();
    }
        
    if(millis() > timeAtStartConnect + CONNECT_TIMEOUT)
    {
      DBG("Could not connect to "+ssid);
      setConnected(false);
      for(int i=0;i<5;i++)
      {
        digitalWrite(13, HIGH);
        delay(50);
        digitalWrite(13, LOW);
        delay(50);
      }
      
      setupLocal();
    }
  }

  void setupLocal()
  {
    String softAPName = "FlowConnect "+Config::instance->getDeviceName();
    String softAPPass = "findyourflow";
    WiFi.softAP(softAPName.c_str(), softAPPass.c_str());
    Serial.println("Local IP : "+String(WiFi.softAPIP()[0])+
    "."+String(WiFi.softAPIP()[1])+
    "."+String(WiFi.softAPIP()[2])+
    "."+String(WiFi.softAPIP()[3]));

    isLocal = true;
    isConnecting = false;
    setConnected(true);
    
    DBG("AP WiFi is init with name "+softAPName+" , pass : "+softAPPass);
  }

  void setConnected(bool value)
  {
    isConnected = value;
    onConnectionUpdate();
  }


  typedef void(*onConnectionUpdateEvent)();
    void (*onConnectionUpdate) ();

    void setCallbackConnectionUpdate (onConnectionUpdateEvent func) {
      onConnectionUpdate = func;
    }

    static void connectionUpdateDefaultCallback()
    {
      //nothing
    }
};
