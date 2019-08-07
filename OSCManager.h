#pragma once

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <ESPmDNS.h>
#include "SerialManager.h"

class OSCManager :
  public CommandProvider
{
public:
  OSCManager() : CommandProvider("OSC") {}
  ~OSCManager(){}
    
  WiFiUDP Udp;
  const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)
  char address[64];
   
  void init()
  {    
    Udp.begin(localPort);
    char buf[16];
    sprintf(buf, "IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
    DBG("OSC Initialized, listening on "+String(buf)+":"+String(localPort));

    if (!MDNS.begin("connectbridge")) {
        Serial.println("Error setting up MDNS responder!");
    }else
    {
        MDNS.addService("_osc", "_udp", localPort);
        Serial.println("mDNS responder started");
    }
  }
  
  void update() {
    int size = Udp.parsePacket();
    
    if (size > 0) {
      OSCMessage msg;
      
      while (size--) {
        msg.fill(Udp.read());
      }
      
      if (!msg.hasError()) 
      {     

        Serial.println("Received message !");
        if(msg.fullMatch("/wakeUp"))
        {
            sendCommand(WAKEUP);
        }else if(msg.fullMatch("/powerOff"))
        {
            sendCommand(POWEROFF);
        }
        else if(msg.fullMatch("/pattern"))
        {
          PatternData p;
          
          p.groupID = msg.getInt(0);
          p.page = msg.getInt(1);
          p.mode = msg.getInt(2);

          p.actives = msg.getInt(3);
          
          p.hueOffset = msg.getInt(4);
          p.saturation = msg.getInt(5);
          p.brightness = msg.getInt(6);
          p.speed = msg.getInt(7);
          p.density = msg.getInt(8);
          
          p.lfo1 = msg.getInt(9);
          p.lfo2 = msg.getInt(10);
          p.lfo3 = msg.getInt(11);
          p.lfo4 = msg.getInt(12);

          sendPattern(p);
        } else if(msg.fullMatch("/wifiSettings"))
        {
           CommandData d;
            d.type = SET_WIFI_CREDENTIALS;
            msg.getString(0,d.value1.stringValue);
            msg.getString(1,d.value2.stringValue);
            //sendCommand(d);
        }else if(msg.fullMatch("/play"))
        {
           CommandData d;
            d.type = PLAY_SHOW;
            msg.getString(0,d.value1.stringValue);
            sendCommand(d);
        }else if(msg.fullMatch("/stop"))
        {
           sendCommand(STOP_SHOW);
        }else if(msg.fullMatch("/pause"))
        {
           sendCommand(PAUSE_SHOW);
        }else if(msg.fullMatch("/resume"))
        {
           sendCommand(RESUME_SHOW);
        }else if(msg.fullMatch("/seek"))
        {
           CommandData d;
            d.type = SEEK_SHOW;
            d.value1.floatValue = msg.getFloat(0);
            sendCommand(d);
        
        
        /*else if(msg.fullMatch("/group"))
        {
           CommandData d;
            d.type = SET_GROUP;
            d.value1.intValue = msg.getInt(0);
            sendCommand(d);
        }else if(msg.fullMatch("/page"))
        {
          CommandData d;
            d.type = SET_PAGE;
            d.value1.intValue = msg.getInt(0);
            sendCommand(d);
        }else if(msg.fullMatch("/setAll"))
        {
          CommandData d;
            d.type = SET_ALL;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            d.value3.intValue = msg.getInt(2);
            sendCommand(d);
        }else if(msg.fullMatch("/pageMode"))
        {
          CommandData d;
            d.type = SET_PAGEMODE;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            sendCommand(d);
        }else if (msg.fullMatch("/mode"))
        {
         CommandData d;
            d.type = SET_MODE;
            d.value1.intValue = msg.getInt(0);
            sendCommand(d);
        }else if(msg.fullMatch("/nextPage"))
        {
          sendCommand(NEXT_PAGE);
        }else if(msg.fullMatch("/nextMode"))
        {
          sendCommand(NEXT_MODE);
        } else if(msg.fullMatch("/lfo"))
        {
           CommandData d;
            d.type = SET_LFO;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            sendCommand(d);
        } else if(msg.fullMatch("/intensity"))
        {
           CommandData d;
            d.type = SET_INTENSITY;
            d.value1.intValue = msg.getInt(0);
            sendCommand(d);
        } else if(msg.fullMatch("/hsv"))
        {
           CommandData d;
            d.type = SET_HSV;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            d.value3.intValue = msg.getInt(2);
            sendCommand(d);
        } else if(msg.fullMatch("/speedDensity"))
        {
           CommandData d;
            d.type = SET_SPEEDDENSITY;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            sendCommand(d);
        } else if(msg.fullMatch("/palette"))
        {
           CommandData d;
            d.type = SET_PALETTE;
            d.value1.intValue = msg.getInt(0);
            sendCommand(d);
        }else if(msg.fullMatch("/sync"))
        {
           sendCommand(SYNC_RF);
        }*/
       }else{
          char addr[32];
          msg.getAddress(addr, 0);
          DBG("OSC Address not handled : "+String(addr));
        }
      }
    }
  }
};
