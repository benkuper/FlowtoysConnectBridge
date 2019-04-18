#define SERIAL_DEBUG 1
#define SERIAL_SYNC 1

#include "Config.h"
#include "RFManager.h"
#include "WifiManager.h"
#include "OSCManager.h"
#include "SerialManager.h"
#include "ButtonManager.h"
#include "LedManager.h"

SerialManager serialManager;
Config conf;
RFManager rfManager;
WifiManager wifiManager;
OSCManager oscManager;
ButtonManager btManager;
LedManager ledManager;

void setup()
{

  serialManager.init();
  conf.init();
  rfManager.init();
  btManager.init();
  wifiManager.init();
  oscManager.init();
  ledManager.init();

  serialManager.setCommandCallback(&commandCallback);
  btManager.setCommandCallback(&commandCallback);
  oscManager.setCommandCallback(&commandCallback);

  rfManager.setRFDataCallback(&rfDataCallback);

  DBG("Bridge is initialized");
}


void loop()
{
  btManager.update();
  serialManager.update();
  oscManager.update();
  rfManager.update();

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    CRGB col = CRGB::Black;
    if (btManager.veryLongPress[i]) col = CRGB::Red;
    else if (btManager.longPress[i]) col = CRGB::Orange;
    else if (btManager.pressed[i]) col = CRGB::Green;

    if (btManager.multipressCount[i] > 1) col = CHSV(btManager.multipressCount[i] * 20, 255, 255);
    ledManager.setLed(i, col);
  }
}


void commandCallback(String providerId, CommandProvider::CommandData data)
{
  DBG("Got Command from " + providerId + " : " + data.type);
  switch (data.type)
  {
    case CommandProvider::CommandType::WAKEUP: rfManager.wakeUp(); break;
    case CommandProvider::CommandType::POWEROFF: rfManager.powerOff(); break;
    case CommandProvider::CommandType::SET_ALL: rfManager.setAll(data.value1.intValue, data.value2.intValue, data.value3.intValue, true); break;
    case CommandProvider::CommandType::SET_PAGEMODE: rfManager.setPageMode(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::SET_MODE: rfManager.setMode(data.value1.intValue); break;
    case CommandProvider::CommandType::SET_PAGE: rfManager.setPage(data.value1.intValue); break;
    case CommandProvider::CommandType::SET_ADJUST: rfManager.setAjdust(data.value1.intValue); break;
    case CommandProvider::CommandType::SET_LFO: rfManager.setLFO(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::SET_SEED: rfManager.setSeed(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::NEXT_MODE: rfManager.nextMode(); break;
    case CommandProvider::CommandType::NEXT_PAGE: rfManager.nextPage(); break;
    case CommandProvider::CommandType::SYNC_RF: rfManager.syncRF(); break;
    case CommandProvider::CommandType::SET_GROUP: rfManager.joinRF(data.value1.intValue); break;
    case CommandProvider::CommandType::SET_WIFI_CREDENTIALS:
      {
        DBG("Set Wifi credentials : " + String(data.value1.stringValue) + ":" + String(data.value2.stringValue));
        conf.setWifiSSID(data.value1.stringValue);
        conf.setWifiPassword(data.value2.stringValue);
        wifiManager.connect();
      }
      break;
    default: DBG("Command not handled"); break;
  }

}



void rfDataCallback()
{

  //DBG("RF Data callback");
#if(SERIAL_SYNC)
  serialManager.sendTrigger("Changed");
  serialManager.sendIntValue("Page", rfManager.sync_pkt.page);
  serialManager.sendIntValue("Mode", rfManager.sync_pkt.mode);
  serialManager.sendIntValue("WakeUp", rfManager.sync_pkt.wakeup);
  serialManager.sendIntValue("PowerOff", rfManager.sync_pkt.poweroff);
  serialManager.sendIntValue("LFO-0", rfManager.sync_pkt.adjust.lfo[0]);
  serialManager.sendIntValue("LFO-1", rfManager.sync_pkt.adjust.lfo[1]);
  serialManager.sendIntValue("Seed-0", rfManager.sync_pkt.adjust.seeds[0]);
  serialManager.sendIntValue("Seed-1", rfManager.sync_pkt.adjust.seeds[1]);
  serialManager.sendIntValue("Adjustment", rfManager.sync_pkt.adjust_active);
#endif
}
