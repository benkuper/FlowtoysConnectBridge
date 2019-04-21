#include "RF24.h"
#include "CommandProvider.h"

#pragma pack(push, 1) // prevents memory alignment from disrupting the layout and size of the network packet 
struct SyncPacket {
  uint16_t groupID;
  uint32_t padding;
  uint8_t lfo_active : 1;
  uint8_t global_active : 1;
  uint8_t lfo[4];
  uint8_t global_intensity;
  uint8_t global_hue;
  uint8_t global_sat;
  uint8_t global_val;
  uint8_t global_palette;
  uint8_t global_speed;
  uint8_t global_density;
  uint8_t page;
  uint8_t mode;
  uint8_t adjust_active : 1;
  uint8_t wakeup : 1;
  uint8_t poweroff : 1;
  uint8_t force_reload : 1;
  uint8_t save : 1;
  uint8_t del : 1;
  uint8_t alternate : 1;
};
#pragma pack(pop)


class RFGroup
{
  public:
    RFGroup() {}

    ~RFGroup() {}

    void setup(int gid, RF24 * r)
    {
      groupID = gid;
      radio = r;
      packet.padding = 10000;
      packet.groupID = ((gid & 0xff) << 8) | ((gid >> 8) & 0xff);
    
      //address[0] = groupID & 0xff;
      //address[1] = (groupID >> 8) & 0xff;
    }

    RF24 * radio = nullptr;
    int groupID = -1;
    SyncPacket packet;
    
    void setRadio()
    {
      //DBG("Set radio "+String(address[0])+ " / "+String(address[1]));
      /*
      radio->stopListening();
      radio->setPayloadSize(sizeof(packet));
      radio->openReadingPipe(1, address);
      radio->openWritingPipe(address);
      radio->startListening();
      */
    }

    void sendPacket()
    {
      if(groupID == -1) return;
      
      //setRadio();
      DBG("Send to group " + String(groupID) + ", padding " + String(packet.padding)+", page "+String(packet.page)+", mode "+String(packet.mode));
      //for(int i=0;i<20;i++)
      //{
        radio->write(&packet, sizeof(SyncPacket));
      //  delay(2);
     // }
    }
    
    void setData(CommandProvider::PatternData data)
    {
      packet.padding++;
      
      packet.page = data.page;
      packet.mode = data.mode;

      packet.lfo_active = true;
      packet.global_active = true;
      packet.lfo[0] = data.lfo1;
      packet.lfo[1] = data.lfo2;
      packet.lfo[2] = data.lfo3;
      packet.lfo[3] = data.lfo4;
      packet.global_intensity = data.brightness;
      packet.global_hue = data.hueOffset;
      packet.global_sat = data.saturation;
      packet.global_palette = 0;
      packet.global_speed = data.speed;
      packet.global_density = data.density;

      DBG("Set Pattern, groupID = "+String(groupID)+", padding = "+packet.padding);
    }

     void updateFromPacket(SyncPacket receivingPacket)
     {
      packet.padding = receivingPacket.padding;
      //packet.page = receivingPacket.page;
      //packet.mode = receivingPacket.mode;

/*
      packet.lfo_active = true;
      packet.global_active = true;
      packet.lfo[0] = data.lfo1;
      packet.lfo[1] = data.lfo2;
      packet.lfo[2] = data.lfo3;
      packet.lfo[3] = data.lfo4;
      packet.global_intensity = data.brightness;
      packet.global_hue = data.hueOffset;
      packet.global_sat = data.saturation;
      packet.global_palette = 0;
      packet.global_speed = data.speed;
      packet.global_density = data.density;
      */
     }
};
