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

#define NUM_PUBLIC_GROUPS 5
#define PUBLIC_GROUP_START_ID 1

#define MAX_PRIVATE_GROUPS 32

#define AUTOADD_PRIVATES 1

#define SEND_TIME 30 //ms

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

    int numActivePrivateGroups;
    
    bool radioIsConnected; //to implement

    void init()
    {
      for (int i = 0; i < NUM_PUBLIC_GROUPS; i++) publicGroups[i].setup(PUBLIC_GROUP_START_ID + i, &radio);

      numActivePrivateGroups = 0;
      for (int i = 0; i < MAX_PRIVATE_GROUPS; i++) 
      {
        privateGroups[i].setup(Config::instance->getRFNetworkId(i), &radio);
        if(privateGroups[i].groupID > 0) privateGroups[i]
      }

      setRFDataCallback(&RFManager::onRFDataDefaultCallback);
      setupRadio();
    }

    void update()
    {
      if (millis() > lastSendTime + SEND_TIME)
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

#if SERIAL_DEBUG
      radio.printDetails();

#endif

    }

    void sendPackets()
    {
      radio.stopListening();
      for (int i = 0; i < NUM_PUBLIC_GROUPS; i++) publicGroups[i].sendPacket();
      for (int i = 0; i < MAX_PRIVATE_GROUPS; i++) privateGroups[i].sendPacket();
      radio.startListening();
    }



    void setPattern(CommandProvider::PatternData data)
    {
      if (data.groupID >= 1 && data.groupID <= 5) //public groups
      {
        publicGroups[data.groupID - 1].setData(data);
      } else
      {
        //private group
      }
    }
    
    //SEND / RECEIVE
    bool receivePacket() {

      if ( radio.available()) {

        while (radio.available()) {
          radio.read(&receivingPacket, sizeof(SyncPacket));

          //reverse group bytes because address is reversed in rf packet but we read end of address as data to get groupID
          
          receivingPacket.groupID = (receivingPacket.groupID >> 8 & 0xff) | ((receivingPacket.groupID & 0xff) << 8);
          DBG("Received packet with groupID : " + String(receivingPacket.groupID) + ", padding " + String(receivingPacket.padding) + ", " + String(receivingPacket.page) + ", " + String(receivingPacket.mode) );

          if (receivingPacket.groupID >= PUBLIC_GROUP_START_ID && receivingPacket.groupID < PUBLIC_GROUP_START_ID + NUM_PUBLIC_GROUPS)
          {
            publicGroups[receivingPacket.groupID - PUBLIC_GROUP_START_ID].updateFromPacket(receivingPacket);
          }else
          {
            bool found = false;
            for(int i=0;i<MAX_PRIVATE_GROUPS;i++)
            {
              if(receivingPacket.groupID == privateGroups[i].groupID)
              {
                privateGroups[i].updateFromPacket(receivingPacket);
                found = true;
                break;
              }
            }

            if(!found)
            {
              DBG("Packet from unknown group received "+String(receivingPacket.groupID));
            }
          }

          onRFData();
        }

        return true;
      }

      return false;
    }

    void resetPrivateGroups()
    {
      
    }


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

    //DATA SYNC
    typedef void(*RFEvent)();
    void (*onRFData) ();
    void setRFDataCallback (RFEvent func) {
      onRFData = func;
    }
    static void onRFDataDefaultCallback() {}
};
