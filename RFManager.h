#pragma once

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#include "Config.h"
#include "SerialManager.h"


/*
 * 
 * GROUP IDS (may change to 1-5)
 * purple = 2
blue = 3
aqua =  4
green = 5
orange = 6

 */

class RFManager
{
  public:
    RFManager() : radio(4,33)
    {
      setRFDataCallback(&RFManager::onRFDataDefaultCallback);
    }
    
    ~RFManager() {}

    int currentGroup;
    
    uint8_t addrJ[5] = { 0x11, 0x50, 0x01, 0x07, 0xf1 }; // base adress for Flowtoys props,
    // first 2 bytes are subject to change depending on joined network

    RF24 radio; //CE and CS pins for SPI bus on NRF24+
    unsigned long age = 0;
    bool sync_pkt_changed = false;
    bool is_on = false;
    uint32_t current_counter = 0;

    //STRUCTURES

    //Network ID
    struct rfNetId {
      uint32_t id;
    } currentNetworkId;

    struct patterns_adjust {
      uint8_t lfo[2];
      uint32_t seeds[2];
    };

#pragma pack(push, 1) // prevents memory alignment from disrupting the layout and size of the network packet
    struct syncData {
      uint32_t padding;
      struct patterns_adjust adjust;
      uint8_t page;
      uint8_t mode;
      uint8_t adjust_active : 1;
      uint8_t wakeup : 1;
      uint8_t poweroff : 1;
    } sync_pkt;
#pragma pack(pop)


    void init()
    {

      currentGroup = 3;
      
      radio.begin();
      radio.setAutoAck(false);
      //  radio.setDataRate(RF24_2MBPS);
      radio.setDataRate(RF24_250KBPS);
      radio.setChannel(2);
      radio.setAddressWidth(5);
      radio.setPayloadSize(sizeof(sync_pkt));
      radio.setCRCLength(RF24_CRC_16);
      //  radio.setCRCLength(RF24_CRC_8);
      //    radio.disableCRC();
      //    radio.enableDynamicPayloads();


      currentNetworkId.id = Config::instance->getRFNetworkId();
      joinRF(currentNetworkId.id);

      #if SERIAL_DEBUG
    radio.printDetails();
      #endif

    }

    void update()
    {
      uint32_t now = millis();

      if (sync_pkt_changed) {
        DBG("Sync packet changed, sending packet");
        sendPacket(2,20);
        age = now;
      } else if (is_on && now - age > 2000) {
        sendPacket();
        age = now;
      }

      receivePacket();
    }


    //NETWORK FUNCTIONS
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


    //SEND / RECEIVE
    bool receivePacket() {

      if ( radio.available()) {

        while (radio.available()) {
          radio.read(&sync_pkt, sizeof(sync_pkt));

          
          if (sync_pkt.padding == current_counter) continue;
          current_counter = sync_pkt.padding;

          DBG("Received packet with padding : " +String(sync_pkt.padding));
          
          if (sync_pkt.wakeup && !sync_pkt.poweroff) {
            is_on = true;
          }
          if (sync_pkt.poweroff && !sync_pkt.wakeup) {
            is_on = false;
          }

          onRFData();
        }

        return true;
      }
      else
        return false;
    }

    void sendPacket(int d = 0, byte repeat = 1)
    {   
      if (sync_pkt_changed) {
        sync_pkt.padding ++;
        sync_pkt_changed = false;
      }      

      DBG("Send packet with padding " +String(sync_pkt.padding));
      
      for (int i = 0; i < repeat; i++) {
        if (i > 0) delay(d);
        radio.stopListening();
        radio.write(&sync_pkt, sizeof(sync_pkt));
        radio.startListening();
      }
    }



    // COMMAND FUNCTIONS

    void wakeUp() {
      DBG("Wake up");
      sync_pkt.wakeup = true;
      sync_pkt_changed = true;
      sendPacket(1, 255);
      sync_pkt.wakeup = false;
      sync_pkt_changed = true;
      is_on = true;
    }

    void powerOff() {
      DBG("Power off");
      sync_pkt.poweroff = true;
      sync_pkt_changed = true;
     sendPacket(1, 255);
      sync_pkt.poweroff = false;
      sync_pkt_changed = true;
      is_on = false;
    }


     void setAll(int group, int page, int mode, bool forceSend)
    {
      joinRF(group);
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
      sync_pkt.adjust_active = (uint8_t)value;
      sync_pkt_changed = true;
    }
    
    void setLFO(int id, uint8_t value)
    {
      if (id < 0 || id >= 2) return;
      DBG("Set LFO "+String(id)+" : "+String(value));
      sync_pkt.adjust.lfo[id] = value;
      sync_pkt_changed = true;
    }

    void setSeed(int id, uint32_t value)
    {
      if (id < 0 || id >= 2) return;
      DBG("Set Seed "+String(id)+" : "+String(value));
      sync_pkt.adjust.seeds[id] = value;
      sync_pkt_changed = true;
    }

    void nextPage() {
      setPage((sync_pkt.page + 1) % 3);
    }
    void nextMode() {
      setMode((sync_pkt.mode + 1) % 10);
    }


    //DATA SYNC
    typedef void(*RFEvent)();
    void (*onRFData) ();
    void setRFDataCallback (RFEvent func) { onRFData = func; }
    static void onRFDataDefaultCallback() {}
};
