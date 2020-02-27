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

    DBG("Init OSC");
    
    Udp.stop();
    Udp.begin(localPort);
    
    if (!MDNS.begin("flowtoysconnect")) {
        DBG("Error setting up MDNS responder!");
    }else
    {
        MDNS.addService("_osc", "_udp", localPort);
        DBG("_osc._udp. flowtoysconnect zeroconf is init.");
    }

    DBG("OSC Initialized");// listening on "+String(buf)+":"+String(localPort));

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
        if(msg.fullMatch("/wakeUp"))
        {
           CommandData d;
            d.type = WAKEUP;
            msg.getString(0,d.value1.stringValue);
            msg.getString(1,d.value2.stringValue);
           d.value1.intValue = msg.getInt(0); //group
           d.value2.intValue = msg.getInt(1); //isPublic
           sendCommand(d);
        }else if(msg.fullMatch("/powerOff"))
        {
            CommandData d;
            d.type = POWEROFF;
            msg.getString(0,d.value1.stringValue);
            msg.getString(1,d.value2.stringValue);
           d.value1.intValue = msg.getInt(0); //group
           d.value2.intValue = msg.getInt(1); //isPublic
           sendCommand(d);
        }else if(msg.fullMatch("/sync"))
        {
            CommandData d;
            d.type = SYNC_RF;
            d.value1.floatValue = msg.getFloat(0);
           sendCommand(d);
        }else if(msg.fullMatch("/stopSync"))
        {
           sendCommand(STOP_SYNC);
        }else if(msg.fullMatch("/resetSync"))
        {
          sendCommand(RESET_SYNC);
        }
        else if(msg.fullMatch("/pattern"))
        {
          PatternData p;
          
          p.groupID = msg.getInt(0);
          p.groupIsPublic = msg.getInt(1);
          p.page = msg.getInt(2);
          p.mode = msg.getInt(3);

          p.actives = msg.getInt(4);
          
          p.hueOffset = msg.getInt(5);
          p.saturation = msg.getInt(6);
          p.brightness = msg.getInt(7);
          p.speed = msg.getInt(8);
          p.density = msg.getInt(9);
          
          p.lfo1 = msg.getInt(10);
          p.lfo2 = msg.getInt(11);
          p.lfo3 = msg.getInt(12);
          p.lfo4 = msg.getInt(13);

          sendPattern(p);
        } else if(msg.fullMatch("/wifiSettings"))
        {
           CommandData d;
            d.type = SET_WIFI_CREDENTIALS;
            msg.getString(0,d.value1.stringValue);
            msg.getString(1,d.value2.stringValue);
            sendCommand(d);
        }else if(msg.fullMatch("/globalConfig"))
        {
            CommandData d;
            d.type = SET_GLOBAL_CONFIG;
            msg.getString(0, d.value1.stringValue);
            d.value2.intValue = msg.size() > 1?msg.getInt(1):2; //0 is wifi, 1 is BLE, 2 is both
            sendCommand(d);
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
