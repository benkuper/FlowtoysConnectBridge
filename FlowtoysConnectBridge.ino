#define SERIAL_DEBUG 1

#include "Config.h"
#include "RFManager.h"
#include "WifiManager.h"
#include "OSCManager.h"
#include "SerialManager.h"
#include "ButtonManager.h"


SerialManager serialManager;
Config conf;
RFManager rfManager;
WifiManager wifiManager;
OSCManager oscManager;
ButtonManager btManager;


void setup(void)
{
  serialManager.init();
  conf.init();
  rfManager.init();
  btManager.init();
  wifiManager.init();
  oscManager.init();

  serialManager.setCommandCallback(&commandCallback);
  btManager.setCommandCallback(&commandCallback);
  oscManager.setCommandCallback(&commandCallback);

  DBG("Bridge is initialized");
}


void loop(void)
{
  serialManager.update();
  oscManager.update();
  rfManager.update();
}


void commandCallback(String providerId, CommandProvider::CommandData data)
{
  //DBG("Got Command from " + providerId + " : " + data.type);
  switch (data.type)
  {
    case CommandProvider::CommandType::WAKEUP: rfManager.wakeUp(); break;
    case CommandProvider::CommandType::POWEROFF: rfManager.powerOff(); break;
    case CommandProvider::CommandType::SET_MODE: rfManager.setMode(data.value1.intValue); break;
    case CommandProvider::CommandType::SET_PAGE: rfManager.setPage(data.value1.intValue); break;
    case CommandProvider::CommandType::NEXT_MODE: rfManager.nextMode(); break;
    case CommandProvider::CommandType::NEXT_PAGE: rfManager.nextPage(); break;
    case CommandProvider::CommandType::SYNC_RF: rfManager.syncRF(); break;
    case CommandProvider::CommandType::SET_WIFI_CREDENTIALS:
    {
      DBG("Set Wifi credentials : "+String(data.value1.stringValue)+":"+String(data.value2.stringValue));
      conf.setWifiSSID(data.value1.stringValue); 
      conf.setWifiPassword(data.value2.stringValue);
      wifiManager.connect();
    }
    break;
    default: DBG("Command not handled"); break;
  }
}

