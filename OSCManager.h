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
    DBG("OSC Initialized");
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
          
        }else if(msg.fullMatch("/powerOff"))
        {
          
        }else if(msg.fullMatch("/page"))
        {
          
        }else if (msg.fullMatch("/mode"))
        {
          
        }else if(msg.fullMatch("/lfo"))
        {
          
        }else if(msg.fullMatch("/seed"))
        {
          
        }

        
        /*
        if (msg.match("/powerOff"))
        {
            rf->sync_pkt.poweroff = 1;
            rf->sync_pkt.wakeup = 0;
            rf->sendPacket(1, 10);
        }else if (msg.match("/powerOn"))
        {
            rf->sync_pkt.poweroff = 0;
            rf->sync_pkt.wakeup = 1;
            rf->sendPacket(1, 255);
          
        } else if (msg.match("/mode"))
        {
          rf->sync_pkt.page = (byte) msg.getInt(0);
          rf->sync_pkt.mode = (byte) msg.getInt(1);
          rf->sendPacket(10, 10);
        }else if (msg.match("/2/push1"))
        {
          #if SERIAL_DEBUG
          Serial.println("Got push 1");
          #endif
          
          if(msg.getFloat(0) > 0)
          {
            rf->sync_pkt.page =  (rf->sync_pkt.page+1)%3;
            rf->sync_pkt_changed = true;
          }
        }else if (msg.match("/2/push2"))
        {
          #if SERIAL_DEBUG
          Serial.println("Got push 2");
          #endif
          if(msg.getFloat(0) > 0)
          {
            rf->sync_pkt.mode =  (rf->sync_pkt.mode+1)%10;
            rf->sync_pkt_changed = true;
          }
        }
         */

         
      }
    }
  }
};
