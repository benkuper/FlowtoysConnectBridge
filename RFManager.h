#pragma once

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#include "Config.h"
#include "SerialManager.h"
#include "RFGroup.h"

/*
   GROUP IDS
  orange = 1
  green = 2
  aqua =  3
  blue = 4
  purple = 5
*/

#define MAX_PRIVATE_GROUPS 10
#define SEND_TIME 1000 //ms

class RFManager
{
  public:
    RFManager() : radio(4, 33) {}
    ~RFManager() {}

    RF24 radio; //CE and CS pins for SPI bus on NRF24+
    RFGroup publicGroups[5];
    RFGroup privateGroups[MAX_PRIVATE_GROUPS];

    uint8_t address[5] = { 0x01, 0x07, 0xf1, 0, 0 };
    SyncPacket receivingPacket;
    
    long lastSendTime = 0;
    
    void init()
    {
      for (int i = 0; i < 5; i++) publicGroups[i].setup(i+1, &radio);

      setRFDataCallback(&RFManager::onRFDataDefaultCallback);
      setupRadio();

    }

    void update()
    {
      if(millis() > lastSendTime + SEND_TIME) 
      {
        sendPackets();
        lastSendTime = millis();
     }
      
      receivePacket();
    }


    void setupRadio()
    {
      radio.begin();
      radio.setAutoAck(false);
      radio.setDataRate(RF24_250KBPS);
      radio.setChannel(2);
      radio.setAddressWidth(3);
      radio.setPayloadSize(sizeof(SyncPacket));
      radio.setCRCLength(RF24_CRC_16);

      radio.stopListening();
      radio.openReadingPipe(1, address);
      radio.openWritingPipe(address);
      radio.startListening();
      
      //radio.disableCRC();
      //radio.enableDynamicPayloads();


      #if SERIAL_DEBUG
      radio.printDetails();
      #endif

    }


    void sendPackets()
    {
      radio.stopListening();
      for(int i=0;i<5;i++) publicGroups[i].sendPacket();  
      radio.startListening();
    }


    
    void setPattern(CommandProvider::PatternData data)
    {
      if(data.groupID >= 1 && data.groupID <= 5) //public groups
      {
        publicGroups[data.groupID-1].setData(data);
      }else
      {
        //private group
      }
    }



    //NETWORK FUNCTIONS
    /*
      bool syncRF()
      {
      DBG("Synchronizing RF, put the props in page 4, mode 0");
      currentNetworkId.id = 0;
      int numTries = 4; //4 cycles for 6 channelss

      for (int i = 0; i < numTries; i++)
      {
        for (int channel = 0; channel < 6; channel++)
        {
          DBG(String("Scanning channel ") + String(channel) + "...");

          addrJ[1] = 0x50;
          addrJ[0] = channel;

          radio.stopListening();
          radio.setPayloadSize(sizeof(rfNetId));
          radio.openReadingPipe(1, addrJ);
          radio.openWritingPipe(addrJ);
          radio.startListening();

          if (!radio.available()) {
            delay(200);
          }

          if (radio.available()) {
            radio.read(&currentNetworkId, sizeof(sync_pkt));

            if (currentNetworkId.id != 0)
            {
              DBG("Found RF Network with id "+ String(currentNetworkId.id));
              Config::instance->setRFNetworkId(currentNetworkId.id);
              joinRF(currentNetworkId.id);
              break;
            }
          }
        }
        if(currentNetworkId.id != 0) break;
      }

      if(currentNetworkId.id == 0)
      {
        DBG("Could not find a RF Network");
      }
      return currentNetworkId.id != 0;
      }


      void joinRF(uint32_t id)
      {
      if(currentGroup == id) return;
      currentGroup = id;

      addrJ[0] = id & 0xff;
      addrJ[1] = (id >> 8) & 0xff;

      DBG("Joining RF Network with id "+String(currentGroup)+ " : "+String(addrJ[0])+", "+String(addrJ[1]));

      radio.stopListening();
      radio.setPayloadSize(sizeof(sync_pkt));
      radio.openReadingPipe(1, addrJ);
      radio.openWritingPipe(addrJ);
      radio.startListening();

      }
    */

    //SEND / RECEIVE
    
    bool receivePacket() {

      if ( radio.available()) {

        while (radio.available()) {
          radio.read(&receivingPacket, sizeof(SyncPacket));

          //reverse group
          receivingPacket.groupID = (receivingPacket.groupID >> 8 & 0xff) | ((receivingPacket.groupID & 0xff) << 8);
          DBG("Received packet with groupID : "+String(receivingPacket.groupID)+", padding "+String(receivingPacket.padding)+", "+String(receivingPacket.page)+", "+String(receivingPacket.mode) );

          if(receivingPacket.groupID >= 1 && receivingPacket.groupID <= 5)
          {
            publicGroups[receivingPacket.groupID-1].updateFromPacket(receivingPacket);
          }
          
          /*
          if (receivePacket.padding == current_counter) continue;
          current_counter = sync_pkt.padding;

          DBG("Received packet with padding : " + String(sync_pkt.padding));

          if (sync_pkt.wakeup && !sync_pkt.poweroff) {
            is_on = true;
          }
          if (sync_pkt.poweroff && !sync_pkt.wakeup) {
            is_on = false;
          }*/

          onRFData();
        }

        return true;
      }

      return false;
    }
    
    /*
    void sendPacket(int d = 0, byte repeat = 1)
    {
      if (sync_pkt_changed) {
        sync_pkt.padding ++;
        sync_pkt_changed = false;
      }

      DBG("Send packet with padding " + String(sync_pkt.padding) + ", lfo active ?" + String(sync_pkt.lfo_active) + ", global active ?" + String(sync_pkt.global_active));

      for (int i = 0; i < repeat; i++) {
        if (i > 0) delay(d);
        radio.stopListening();
        radio.write(&sync_pkt, sizeof(sync_pkt));
        radio.startListening();
      }
    }*/

    

    // COMMAND FUNCTIONS

    void wakeUp() {
      DBG("Wake up");
      /*sync_pkt.wakeup = true;
      sync_pkt_changed = true;
      sendPacket(1, 255);
      sync_pkt.wakeup = false;
      sync_pkt_changed = true;
      is_on = true;*/
    }

    void powerOff() {
      DBG("Power off");
      /*sync_pkt.poweroff = true;
      sync_pkt_changed = true;
      sendPacket(1, 255);
      sync_pkt.poweroff = false;
      sync_pkt_changed = true;
      is_on = false;*/
    }


    
    /*
      void setAll(int group, int page, int mode, bool forceSend)
      {
      joinRF(group);
      sync_pkt.adjust_active = false;
      sync_pkt.page = page;
      sync_pkt.mode = mode;
      sync_pkt_changed = true;
      if(forceSend) sendPacket(2,20);
      }

      void setPage(int page)
      {
      DBG("Set Page : "+String(page));
      sync_pkt.page = page;
      sync_pkt_changed = true;
      }

      void setMode(int mode)
      {
      DBG("Set Mode : "+String(mode));
      sync_pkt.mode =  mode;
      sync_pkt_changed = true;
      }

      void setPageMode(int page, int mode)
      {
      DBG("Set Page : "+String(page));
      sync_pkt.page = page;
      sync_pkt.mode =  mode;
      sync_pkt_changed = true;
      }

      void setAjdust(bool value)
      {
      sync_pkt.lfo_active = false;
      sync_pkt.adjust_active = (uint8_t)value;
      sync_pkt_changed = true;
      }

      void setLFO(uint8_t lfo1, uint8_t lfo2)
      {
      DBG("Set LFO "+String(lfo1)+", "+String(lfo2));
      sync_pkt.lfo_active = true;
      sync_pkt.lfo[0] = lfo1;
      sync_pkt.lfo[1] = lfo2;
      sync_pkt_changed = true;
      }

      void setIntensity(uint8_t value)
      {
      DBG("Set Intensity "+String(value));
      sync_pkt.global_active = true;
      sync_pkt.global_intensity = value;
      sync_pkt_changed = true;
      }

      void setHSV(uint8_t h, uint8_t s, uint8_t v)
      {
      DBG("Set HSV "+String(h)+", "+String(s)+", "+String(v));
      sync_pkt.global_active = true;
      sync_pkt.global_hue = h;
      sync_pkt.global_sat = s;
      sync_pkt.global_val = v;
      sync_pkt_changed = true;
      }

      void setSpeedDensity(uint8_t speed, uint8_t density)
      {
      DBG("Set Speed density "+String(speed)+", "+String(density));
      sync_pkt.global_active = true;
      sync_pkt.global_speed = speed;
      sync_pkt.global_density = density;
      sync_pkt_changed = true;
      }

      void setPalette(uint8_t palette)
      {
      DBG("Set Speed density "+String(palette));
      sync_pkt.global_active = true;
      sync_pkt.global_palette = palette;
      sync_pkt_changed = true;
      }

      void nextPage() {
      setPage((sync_pkt.page + 1) % 3);
      }
      void nextMode() {
      setMode((sync_pkt.mode + 1) % 10);
      }
    */

    //DATA SYNC
    typedef void(*RFEvent)();
    void (*onRFData) ();
    void setRFDataCallback (RFEvent func) {
      onRFData = func;
    }
    static void onRFDataDefaultCallback() {}
};
