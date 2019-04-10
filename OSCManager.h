#pragma once

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
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
            sendCommand(WAKEUP);
        }else if(msg.fullMatch("/powerOff"))
        {
            sendCommand(POWEROFF);
        }if(msg.fullMatch("/group"))
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
        }else if(msg.fullMatch("/seed"))
        {
          CommandData d;
            d.type = SET_SEED;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.getInt(1);
            sendCommand(d);
        }else if(msg.fullMatch("/sync"))
        {
           sendCommand(SYNC_RF);
        }else{
          char addr[32];
          msg.getAddress(addr, 0);
          DBG("OSC Address not handled : "+String(addr));
        }
      }
    }
  }
};
