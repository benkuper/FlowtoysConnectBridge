#pragma once

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#include "Config.h"
#include "SerialManager.h"
#include "RFGroup.h"
#include <FastLED.h>

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

#define MAX_PRIVATE_GROUPS 100

#define AUTOADD_PRIVATES 1

#define SEND_TIME 30 //ms
#define FORCESEND_TIME 1000

class RFManager :
  public CommandProvider
{
  public:
    RFManager() : CommandProvider("RF"), radio(4, 33) {}
    ~RFManager() {}
  
    RF24 radio; //CE and CS pins for SPI bus on NRF24+
    RFGroup publicGroups[5];
    RFGroup privateGroups[MAX_PRIVATE_GROUPS];
    
    uint8_t address[5] = { 0x01, 0x07, 0xf1, 0, 0 };
    SyncPacket receivingPacket;

    long lastSendTime = 0;
    long lastForceSendTime = 0;
    
    int numActivePrivateGroups;

    bool syncing;
    long syncTime;
    long timeAtSync;
    
    bool radioIsConnected; //to implement
    

    void init()
    {
      syncing = false;
      
      for (int i = 0; i < NUM_PUBLIC_GROUPS; i++) publicGroups[i].setup(PUBLIC_GROUP_START_ID + i, &radio);

      numActivePrivateGroups = Config::instance->getNumPrivateGroups();
      
      for (int i = 0; i <  numActivePrivateGroups; i++) 
      {
        privateGroups[i].setup(Config::instance->getRFNetworkId(i), &radio);
        DBG(" > Loading private group "+String(i+1)+" : "+String(privateGroups[i].groupID));
      }

      setRFDataCallback(&RFManager::onRFDataDefaultCallback);
      setupRadio();

      DBG("RF Manager is init.");
    }

    void update()
    {
      if (millis() > lastSendTime + SEND_TIME)
      {
        lastSendTime = millis();
        sendPackets();
      }

      if (millis() > lastForceSendTime + FORCESEND_TIME)
      {
        lastForceSendTime = millis();
        sendPackets(true);
      }

      if(syncing && syncTime > 0 && millis() > timeAtSync + syncTime)
      {
        stopSync();
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

    void sendPackets(bool force = false)
    {
      radio.stopListening();
      //for (int i = 0; i < NUM_PUBLIC_GROUPS; i++) publicGroups[i].sendPacket(force);
      for (int i = 0; i < numActivePrivateGroups; i++) privateGroups[i].sendPacket(force);
      radio.startListening();
    }


    void setSolidColors(CRGB * colors)
    {
      for(int i=0;i < numActivePrivateGroups ;i++)
      {
        CommandProvider::PatternData data = CommandProvider::getSolidColorPattern(colors[i]);
        data.brightness *= globalBrightness;
        privateGroups[i].setData(data, true);
      }
    }


    void setPattern(CommandProvider::PatternData data)
    {
      if(data.groupID == 0)
      {
        for(int i=0;i<NUM_PUBLIC_GROUPS;i++) publicGroups[i].setData(data);
        for(int i=0;i<numActivePrivateGroups;i++) privateGroups[i].setData(data);
      }else
      {
        int index = data.groupID - 1;
        if (data.groupIsPublic) //public groups
        {
          if(index >= 0 && NUM_PUBLIC_GROUPS) publicGroups[index].setData(data);
        } else
        {
          if(index >= 0 && numActivePrivateGroups) privateGroups[index].setData(data);
        }
      }
    }

    void wakeUp(int groupID, bool groupIsPublic) {
       if(groupID == 0)
      {
        for(int i=0;i<NUM_PUBLIC_GROUPS;i++) publicGroups[i].wakeUp();
        for(int i=0;i<numActivePrivateGroups;i++) privateGroups[i].wakeUp();
      }else
      {
        int index = groupID - 1;
        if (groupIsPublic) //public groups
        {
          if(index >= 0 && NUM_PUBLIC_GROUPS) publicGroups[index].wakeUp();
        } else
        {
          if(index >= 0 && numActivePrivateGroups) privateGroups[index].wakeUp();
        }
      }
    }

    void powerOff(int groupID, bool groupIsPublic) {
      if(groupID == 0)
      {
        for(int i=0;i<NUM_PUBLIC_GROUPS;i++) publicGroups[i].powerOff();
        for(int i=0;i<numActivePrivateGroups;i++) privateGroups[i].powerOff();
      }else
      {
        int index = groupID - 1;
        if (groupIsPublic) //public groups
        {
          if(index >= 0 && NUM_PUBLIC_GROUPS) publicGroups[index].powerOff();
        } else
        {
          if(index >= 0 && numActivePrivateGroups) privateGroups[index].powerOff();
        }
      }
    }
    
    //SEND / RECEIVE
    bool receivePacket() {

      if ( radio.available()) {

        while (radio.available()) {
          radio.read(&receivingPacket, sizeof(SyncPacket));

          //reverse group bytes because address is reversed in rf packet but we read end of address as data to get groupID
          
          receivingPacket.groupID = (receivingPacket.groupID >> 8 & 0xff) | ((receivingPacket.groupID & 0xff) << 8);
          
          if (receivingPacket.groupID >= PUBLIC_GROUP_START_ID && receivingPacket.groupID < PUBLIC_GROUP_START_ID + NUM_PUBLIC_GROUPS)
          {
            publicGroups[receivingPacket.groupID - PUBLIC_GROUP_START_ID].updateFromPacket(receivingPacket);
          }else
          {
            bool found = false;
            for(int i=0;i<numActivePrivateGroups;i++)
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
              if(syncing)
              {
                bool syncingPage = 1; //page 2
                bool syncingMode = 0; //mode 1
                bool acceptAll = false;
                if(acceptAll || (receivingPacket.page == syncingPage && receivingPacket.mode == syncingMode))
                {
                   if(numActivePrivateGroups < MAX_PRIVATE_GROUPS) 
                    {
                      DBG("Adding group : "+String(receivingPacket.groupID)+" at index "+String(numActivePrivateGroups));
                      digitalWrite(13,HIGH);
                      delay(50);
                      digitalWrite(13,LOW);
                      privateGroups[numActivePrivateGroups].setup(receivingPacket.groupID, &radio);
                      privateGroups[numActivePrivateGroups].updateFromPacket(receivingPacket);
                      Config::instance->setRFNetworkId(numActivePrivateGroups, receivingPacket.groupID);
                      sendCommand(GROUP_ADDED);
                      numActivePrivateGroups++;
                   }else
                   {
                      DBG("Max groups reached");
                   }
                }
               
              }else if(!syncing)
              {
                //DBG("Packet from unknown group received "+String(receivingPacket.groupID));
              }
            }
          }

          onRFData();
        }

        return true;
      }

      return false;
    }

    void syncRF(float timeout = 0)
    {
      syncTime = timeout*1000;
      DBG("Start Sync with timeout :"+String(syncTime));
      timeAtSync = millis();
      syncing = true;
    }

    void stopSync()
    {
      syncing = false;
      Config::instance->setNumPrivateGroups(numActivePrivateGroups);
      DBG("Finish sync, got "+String(Config::instance->getNumPrivateGroups())+" groups");
    }

    void resetSync()
    {
      resetPrivateGroups();
    }

    void resetPrivateGroups()
    {
      DBG("Reset private groups !");
      numActivePrivateGroups = 0;
    }


    // COMMAND FUNCTIONS

    

    //DATA SYNC
    typedef void(*RFEvent)();
    void (*onRFData) ();
    void setRFDataCallback (RFEvent func) {
      onRFData = func;
    }
    static void onRFDataDefaultCallback() {}
};
