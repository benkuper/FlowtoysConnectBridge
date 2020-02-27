#include "Config.h"
Config conf;

#define USE_SERIAL 1
#if USE_SERIAL
#define SERIAL_DEBUG 1
#define USE_BLE 1
#endif

#define USE_RF 1

#define USE_WIFI 1
#if USE_WIFI
#define USE_OSC 1
#endif

#define USE_BUTTONS 1
#define USE_LEDS 1
#define USE_FILES 0
#define USE_PLAYER 0

#if USE_SERIAL
#include "SerialManager.h"
SerialManager serialManager;

#if USE_BLE
#include "BLEManager.h"
BLEManager bleManager;
#endif //BLE
#endif //SERIAL

#if USE_RF
#include "RFManager.h"
RFManager rfManager;
#endif

#if USE_WIFI
#include "WifiManager.h"
WifiManager wifiManager;
#if USE_OSC
#include "OSCManager.h"
OSCManager oscManager;
#endif //OSC
#endif //WIFI

#if USE_BUTTONS
#include "ButtonManager.h"
ButtonManager btManager;
#endif

#if USE_LEDS
#include "LedManager.h"
LedManager ledManager;
#endif

#if USE_FILES
#include "FileManager.h"
FileManager fileManager;
#endif

#if USE_PLAYER
#include "Player.h"
Player player;
#endif


//LEDS
float timeAtLastBLEReceived;
float timeAtLastOSCReceived;
float timeAtLastRFReceived;

void patternCallback(String providerId, CommandProvider::PatternData data)
{
  //DBG("Set pattern ! " + String(data.page) + ":" + String(data.mode));

  if (providerId == "OSC") timeAtLastOSCReceived = millis() / 1000.0f;
  else timeAtLastBLEReceived = millis() / 1000.0f;

#if USE_RF
  rfManager.setPattern(data);
#endif
}

void commandCallback(String providerId, CommandProvider::CommandData data)
{
  DBG("Got Command from " + providerId + " : " + data.type);

  if (providerId == "OSC") timeAtLastOSCReceived = millis() / 1000.0f;
  else timeAtLastBLEReceived = millis() / 1000.0f;

  switch (data.type)
  {
#if USE_RF
    case CommandProvider::CommandType::GROUP_ADDED:
      timeAtLastRFReceived = millis() / 1000.0f;
      break;

    case CommandProvider::CommandType::SYNC_RF:
      {
        rfManager.resetSync(); //tmp because app doesn't have button
        rfManager.syncRF(data.value1.floatValue);
      }
      break;
    case CommandProvider::CommandType::STOP_SYNC: rfManager.stopSync(); break;
    case CommandProvider::CommandType::RESET_SYNC: rfManager.resetSync(); break;
    case CommandProvider::CommandType::WAKEUP: rfManager.wakeUp(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::POWEROFF: rfManager.powerOff(data.value1.intValue, data.value2.intValue); break;
#endif


#if USE_PLAYER
    case CommandProvider::CommandType::PLAY_SHOW: player.play(data.value1.stringValue); break;
    case CommandProvider::CommandType::PAUSE_SHOW: player.pause(); break;
    case CommandProvider::CommandType::STOP_SHOW: player.stop(); break;
    case CommandProvider::CommandType::RESUME_SHOW: player.resume(); break;
    case CommandProvider::CommandType::SEEK_SHOW: player.seek(data.value1.floatValue); break;
#endif


#if USE_WIFI
    case CommandProvider::CommandType::SET_WIFI_CREDENTIALS:
      {
        DBG("Set Wifi credentials : " + String(data.value1.stringValue) + ":" + String(data.value2.stringValue));
        conf.setWifiSSID(data.value1.stringValue);
        conf.setWifiPassword(data.value2.stringValue);
        wifiManager.init();
      }
      break;
#endif

    case CommandProvider::CommandType::SET_GLOBAL_CONFIG:
      {
        String deviceName = String(data.value1.stringValue).equals("*") ? "" : data.value1.stringValue;
        conf.setDeviceName(deviceName);
        conf.setWifiBLEMode(data.value2.intValue);

        DBG("Set Device name : " + conf.getDeviceName() + " and mode wifi : " + String(conf.getWifiMode()) + ", BLE : " + String(conf.getBLEMode()));
        FastLED.delay(500);
        ESP.restart();
      }
      break;

    default: DBG("Command not handled"); break;
  }

}

#if USE_WIFI
void wifiConnectionUpdate()
{
  DBG("Wifi connection update " + String(wifiManager.isConnected));

  if (wifiManager.isConnected)
  {

#if USE_OSC
    DBG("Setup OSC now");
    oscManager.init();
#endif
  }
}
#endif


#if USE_BUTTONS
void handlePress(int id, bool value)
{
  DBG("Pressed " + String(id) + ":" + String(value));
#if USE_LEDS

#endif
}

void handleShortPress(int id)
{
  DBG("Short press " + String(id));
}

void handleLongPress(int id)
{
  DBG("Long press " + String(id));
}

void handleVeryLongPress(int id)
{
  DBG("Very long press " + String(id));
  sleepESP();
}

void handleMultiPress(int id, int count)
{
  DBG("Multi press " + String(id) + " : " + String(count));
}
#endif


#if USE_RF
void rfDataCallback()
{

  //DBG("RF Data callback");
  /*
    #if(SERIAL_SYNC)
    serialManager.sendTrigger("Changed");
    serialManager.sendIntValue("Page", rfManager.sync_pkt.page);
    serialManager.sendIntValue("Mode", rfManager.sync_pkt.mode);
    serialManager.sendIntValue("WakeUp", rfManager.sync_pkt.wakeup);
    serialManager.sendIntValue("PowerOff", rfManager.sync_pkt.poweroff);
    serialManager.sendIntValue("LFO-0", rfManager.sync_pkt.lfo[0]);
    serialManager.sendIntValue("LFO-1", rfManager.sync_pkt.lfo[1]);
    serialManager.sendIntValue("LFO-Active", rfManager.sync_pkt.lfo_active);
    serialManager.sendIntValue("Global-Active", rfManager.sync_pkt.global_active);
    serialManager.sendIntValue("Hue", rfManager.sync_pkt.global_hue);
    serialManager.sendIntValue("Saturation", rfManager.sync_pkt.global_sat);
    serialManager.sendIntValue("Value", rfManager.sync_pkt.global_val);
    serialManager.sendIntValue("Intensity", rfManager.sync_pkt.global_intensity);
    serialManager.sendIntValue("Speed", rfManager.sync_pkt.global_speed);
    serialManager.sendIntValue("Density", rfManager.sync_pkt.global_density);
    #endif
  */
}
#endif

void sleepESP()
{
  for (int i = 255; i >= 0; i--)
  {
    CHSV c(30, 255, i);
    ledManager.setLed(0, c);
    ledManager.setLed(1, c);
    FastLED.delay(2);
  }

  DBG("Sleep !");
  FastLED.delay(500);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_23, LOW);
  esp_deep_sleep_start();
}

// ------------------------------ LEDS

void updateLeds()
{
  float curTime = millis() / 1000.0f;

  CRGB c1 = CRGB::Black;
  CRGB c2 = CRGB::Black;

  if (rfManager.syncing)
  {
    float rp = max(1 - (curTime - timeAtLastRFReceived) / .3f, 0.f);
    c1 = blend(CRGB::Blue, CRGB::Orange, (int)(rp * 255));
    c2 = c1;
  } else
  {
    if (wifiManager.isConnecting)  c1 = CRGB::Yellow;
    else if (wifiManager.isConnected)
    {
      if (wifiManager.isLocal) c1 = CRGB::Purple;
      else c1 = CRGB::Green;
    }

    if (bleManager.isActivated)
    {
      if (bleManager.deviceConnected) c2 = CRGB::Green;
      else c2 = CRGB::Yellow;
    }


    float p1 = max(1 - (curTime - timeAtLastOSCReceived) / .3f, 0.f);
    float p2 = max(1 - (curTime - timeAtLastBLEReceived) / .3f, 0.f);
    c1 = blend(c1, CRGB::White, (int)(p1 * 255));
    c2 = blend(c2, CRGB::White, (int)(p2 * 255));
  }

  ledManager.setLed(0, c1, false);
  ledManager.setLed(1, c2, true);
}


//  ------------------------------   SETUP AND LOOP


void setup()
{
  //Need to activate mosfet
  pinMode(12, OUTPUT);
  digitalWrite(27, LOW);

  conf.init();

#if USE_SERIAL
  serialManager.init();
  serialManager.setCommandCallback(&commandCallback);
  serialManager.setPatternCallback(&patternCallback);
#endif //SERIAL

#if USE_RF
  rfManager.init();
  rfManager.setRFDataCallback(&rfDataCallback);
#endif

#if USE_BLE && USE_SERIAL
  bleManager.init();
#endif //BLE

#if USE_LEDS
  ledManager.init();
#endif

#if USE_WIFI
  wifiManager.init();
  wifiManager.setCallbackConnectionUpdate(wifiConnectionUpdate);

#if USE_OSC
  //wait for wifi event to init
  oscManager.setCommandCallback(&commandCallback);
  oscManager.setPatternCallback(&patternCallback);
#endif //OSC
#endif //WIFI

#if USE_BUTTONS
  btManager.init();
  btManager.setEventCallbacks(handlePress, handleShortPress, handleLongPress, handleVeryLongPress, handleMultiPress);
#endif


#if USE_FILES
  fileManager.init();
#endif

#if USE_PLAYER
  player.init();
#endif

  DBG("Bridge is initialized");
}


void loop()
{
#if USE_BUTTONS
  btManager.update();
#endif

#if USE_SERIAL
  serialManager.update();
#if USE_BLE
  bleManager.update();
#endif
#endif

#if USE_WIFI
  wifiManager.update();
#if USE_OSC
  oscManager.update();
#endif
#endif

#if USE_RF
  rfManager.update();
#endif

#if USE_PLAYER
  player.update();
#endif

#if USE_LEDS
  updateLeds();
#endif

}
