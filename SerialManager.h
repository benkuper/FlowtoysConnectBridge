#pragma once

#include "CommandProvider.h"

#if SERIAL_DEBUG
#define DBG(msg) Serial.println(msg)
#else
#define DBG(msg)
#endif

class SerialManager :
  public CommandProvider
{
  public:
    SerialManager() :
      CommandProvider("Serial")
    {
      instance = this;
    }

    ~SerialManager() {}

    static SerialManager * instance;
    char buffer[256];
    int bufferIndex = 0;

    void init()
    {
      Serial.begin(115200);
      memset(buffer, 0, 32);
    }

    void update()
    {
      while (Serial.available())
      {
        byte c = Serial.read();
        //DBG("Got char : "+String(c));
        if (c == 255 || c == '\n')
        {
          parseMessage();
          memset(buffer, 0, 255);
          bufferIndex = 0;
        } else
        {
          if (bufferIndex < 255) buffer[bufferIndex] = c;
          bufferIndex++;
        }
      }
    }

    void parseMessage()
    {
      char command = buffer[0];
      //DBG("Parse Message,command is : " + String(command));

      switch (command)
      {

         case 's': sendCommand(SYNC_RF); break;

          case 'n':
          {
            CommandData d;
            d.type = SET_WIFI_CREDENTIALS;
            d.value1.stringValue = "flowspace";
            d.value2.stringValue = "flowarts";
            sendCommand(d);
          }
          break;


        case 'w':
          {
            DBG(String("Buffer 1 is ") + (int)buffer[1]);
            if (buffer[1] == 0 || buffer[1] == '0') sendCommand(POWEROFF); //either with bytes or char
            else sendCommand(WAKEUP);
          }
          break;
       
         case 'c': 
         {
          DBG("Got calib command");
          sendCommand(CALIBRATE_BUTTONS);
         }
         break;
         
  
         case 'a':
         {
          CommandData d;
          d.type = PLAY_SHOW;
          d.value1.stringValue = "demo.show";
          sendCommand(d);
         }
         break;


         case 'p':
         case 'P':
         {
          PatternData p;
          
          char * pch;
          pch = strtok (&buffer[1],",");
          String split[13];
          int i=0;
          while (pch != NULL && i < 13)
          {
            split[i] = String(pch);
            pch = strtok (NULL, ",");
            i++;
          }
          
          p.groupID = split[0].toInt();
          p.groupIsPublic = command == 'P';
          
          p.page = split[1].toInt();
          p.mode = split[2].toInt();

          p.actives = split[3].toInt();
          
          p.hueOffset = split[4].toInt();
          p.saturation = split[5].toInt();
          p.brightness = split[6].toInt();
          p.speed = split[7].toInt();
          p.density = split[8].toInt();
          
          p.lfo1 = split[9].toInt();
          p.lfo2 = split[10].toInt();
          p.lfo3 = split[11].toInt();
          p.lfo4 = split[12].toInt();

          sendPattern(p);
         }
         break;
        }
          
        /*case 'j':
          {
            CommandData d;
            d.type = SET_GROUP;
            if (buffer[1] >= 48 && buffer[1] <= 57) buffer[1] -= 48;
            d.value1.intValue = buffer[1];
            sendCommand(d);
          }
          break;
          
        case 'p':
          {
            if (buffer[1] == '+') sendCommand(NEXT_PAGE);
            else
            {
              CommandData d;
              d.type = SET_PAGE;
              if (buffer[1] >= 48 && buffer[1] <= 57) buffer[1] -= 48; //also works with chars 0-9
              d.value1.intValue = buffer[1];
              sendCommand(d);
            }
          }
          break;

        case 'm':
          {
            if (buffer[1] == '+') sendCommand(NEXT_MODE);
            else
            {
              CommandData d;
              d.type = SET_MODE;
              if (buffer[1] >= 48 && buffer[1] <= 57) buffer[1] -= 48; //also works with chars 0-9
              d.value1.intValue = buffer[1];
              sendCommand(d);
            }
          }
          break;

      
        case 'l':
          {
            CommandData d;
            d.type = SET_LFO;
            if (buffer[1] >= 48 && buffer[1] <= 57) buffer[1] -= 48;
            d.value1.intValue = buffer[1];
            d.value2.intValue = buffer[2];
            sendCommand(d);
          }
          break;

        case 'd':
          {
            CommandData d;
            d.type = SET_ADJUST;
            if (buffer[1] >= 48 && buffer[1] <= 57) buffer[1] -= 48;
            d.value1.intValue = buffer[1];
            sendCommand(d);
          }
          break;
        */
      
    }

    void sendTrigger(String name)
    {
      Serial.println(name);
    }

    void sendBoolValue(String name, bool value)
    {
      Serial.println(name + " " + (value ? 1 : 0));
    }

    void sendIntValue(String name, int value)
    {
      Serial.println(name + " " + String(value));
    }

    void sendFloatValue(String name, float value)
    {
      Serial.println(name + " " + String(value));
    }

};

SerialManager * SerialManager::instance = nullptr;
