#include "RF24.h"
#include "CommandProvider.h"

#pragma pack(push, 1) // prevents memory alignment from disrupting the layout and size of the network packet 
struct SyncPacket {
  uint16_t groupID;
  uint32_t padding;
  uint8_t lfo[4];
  uint8_t global_hue;
  uint8_t global_sat;
  uint8_t global_val;
  uint8_t global_speed;
  uint8_t global_density;
  uint8_t lfo_active : 1;
  uint8_t hue_active : 1;
  uint8_t sat_active : 1;
  uint8_t val_active : 1;
  uint8_t speed_active : 1;
  uint8_t density_active : 1;
  uint8_t reserved[2];
  uint8_t page;
  uint8_t mode;
  uint8_t adjust_active : 1;
  uint8_t wakeup : 1;
  uint8_t poweroff : 1;
  uint8_t force_reload : 1;
  uint8_t save : 1;
  uint8_t _delete : 1;
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
      packet.padding = 1;
      packet.groupID = ((gid & 0xff) << 8) | ((gid >> 8) & 0xff);
    }

    RF24 * radio = nullptr;
    int groupID = -1;
    SyncPacket packet;
    
    void sendPacket()
    {
      if(groupID <= 0) return;
      radio->write(&packet, sizeof(SyncPacket));
    }
    
    void setData(CommandProvider::PatternData data)
    {
      packet.padding++;
      
      packet.page = data.page;
      packet.mode = data.mode;
      packet.wakeup = false;
      packet.poweroff = false;
//
//      packet.lfo_active = (data.actives >> 5) & 1;
//      packet.hue_active = data.actives & 1;
//      packet.sat_active = (data.actives >> 1) & 1;
//      packet.val_active = (data.actives >> 2) & 1;
//      packet.speed_active = (data.actives >> 3) & 1;
//      packet.density_active = (data.actives >> 4) & 1;


      packet.lfo_active = data.actives & 1;//true;
      packet.hue_active = (data.actives >> 1) & 1;//true;
      packet.sat_active = (data.actives >> 2) & 1;//true;
      packet.val_active = (data.actives >> 3) & 1;//true;
      packet.speed_active = (data.actives >> 4) & 1;//true;
      packet.density_active = (data.actives >> 5) & 1;//true;

      packet.lfo[0] = data.lfo1;
      packet.lfo[1] = data.lfo2;
      packet.lfo[2] = data.lfo3;
      packet.lfo[3] = data.lfo4;
      packet.global_val = data.brightness;
      packet.global_hue = data.hueOffset;
      packet.global_sat = data.saturation;
      packet.global_speed = data.speed;
      packet.global_density = data.density;

      DBG("Set Pattern, groupID = "+String(groupID)+", padding = "+packet.padding
      +", LFO : "+String(packet.lfo_active)+" > lfo0 : "+String(packet.lfo[0])
      +", Hue active : "+String(packet.hue_active)+" > hue : "+String(packet.global_hue)+", sat : "+String(packet.global_sat)+", val : "+String(packet.global_val)
      +", speed : "+String(packet.global_speed)+", density : "+String(packet.global_density)
      );
  
    }

     void updateFromPacket(SyncPacket receivingPacket)
     {
      if(packet.padding != receivingPacket.padding)
      {
        DBG("Received packet with groupID : " + String(receivingPacket.groupID) + ", padding " + String(receivingPacket.padding));
      }
      
      packet.padding = max(packet.padding,receivingPacket.padding);
      //packet.page = receivingPacket.page;
      //packet.mode = receivingPacket.mode;

      //DBG("LFO Active : "+String(receivingPacket.lfo_active));
      //DBG("Global Active : "+String(receivingPacket.global_active));
      //DBG("Hue : "+String(receivingPacket.global_hue));
      //DBG("Sat : "+String(receivingPacket.global_sat));
      //DBG("Val : "+String(receivingPacket.global_val));
      //DBG("LFO 0 : "+String(receivingPacket.lfo[0]));
           
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

     void wakeUp()
     {
        packet.padding++;
        packet.wakeup = true;
     }

     void powerOff()
     {
        packet.padding++;
        packet.poweroff = true;
            
     }
};
