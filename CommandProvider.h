#pragma once
#include <FastLED.h>

class CommandProvider
{
public:
    CommandProvider(String id) :
      providerId(id)
    {
      setCommandCallback(&CommandProvider::onCommandDefaultCallback);   
      setPatternCallback(&CommandProvider::onPatternDefaultCallback);
    }
    
    virtual ~CommandProvider()
    {
    }

    String providerId;
    enum CommandType { 
      SYNC_RF, STOP_SYNC, RESET_SYNC, GROUP_ADDED, SET_WIFI_CREDENTIALS, SET_GLOBAL_CONFIG,
      WAKEUP, POWEROFF, 
      PLAY_SHOW, PAUSE_SHOW, STOP_SHOW, RESUME_SHOW, SEEK_SHOW, RF_DATA 
      };
    
    union var
    {
      int intValue;
      float floatValue;
      char * stringValue;
    };
      
    struct CommandData
    {
      CommandType type;
      String providerId;
      var value1;
      var value2;
      var value3;
    };

    struct PatternData
    {
      String providerId;
      uint32_t groupID = 0;
      bool groupIsPublic;
      uint8_t page = 0;
      uint8_t mode = 0;

      uint8_t actives = 0;
      
      uint8_t hueOffset = 0;
      uint8_t saturation = 0;
      uint8_t brightness = 0;
        
      uint8_t speed = 0;
      uint8_t density = 0;
      
      uint8_t lfo1 = 0;
      uint8_t lfo2 = 0;
      uint8_t lfo3 = 0;
      uint8_t lfo4 = 0;
    };

    static PatternData getSolidColorPattern(CRGB color)
    {
      PatternData data;
      data.page = 2;
      data.mode = 7;

      CHSV c = rgb2hsv_approximate(color);
      data.actives = 255; //hue, sat, val
      data.hueOffset = c.hue;
      data.saturation = c.sat;
      data.brightness = c.val;
      
      return data;
    }

    void sendCommand(CommandType type) { 
      CommandData data;
      data.type = type;
      sendCommand(data);
    }

    void sendCommand(CommandData data) { onCommand(providerId, data); } 
    
    typedef void(*CommandEvent)(String providerId, CommandData command);
    void (*onCommand) (String providerId, CommandData command);
    void setCommandCallback (CommandEvent func) { onCommand = func; }
    static void onCommandDefaultCallback(String providerId, CommandData command) {}

    void sendPattern(PatternData data) { onPattern(providerId, data); }
 
    typedef void(*PatternEvent)(String providerId, PatternData pattern);
    void (*onPattern) (String providerId, PatternData command);
    void setPatternCallback (PatternEvent func) { onPattern = func; }
    static void onPatternDefaultCallback(String providerId, PatternData command) {}
};
