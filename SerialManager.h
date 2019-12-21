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
          parseMessage(buffer);
          memset(buffer, 0, 255);
          bufferIndex = 0;
        } else
        {
          if (bufferIndex < 255) buffer[bufferIndex] = c;
          bufferIndex++;
        }
      }
    }

    void parseMessage(String message)
    {
      char command = message[0];
      DBG("Parse Message,command is : " + String(command));

      switch (command)
      {
        case 's': sendCommand(SYNC_RF); break;

        case 'n':
          {
            CommandData d;
            d.type = SET_WIFI_CREDENTIALS;
            char * pch;
            pch = strtok (&message[1], ",");
            char* split[2];
            int i = 0;
            while (pch != NULL && i < 2)
            {
              split[i] = pch;
              pch = strtok (NULL, ",");
              i++;
            }
            d.value1.stringValue = split[0];
            d.value2.stringValue = split[1];
            sendCommand(d);
          }
          break;


        case 'w':
        case 'W':
        case 'z':
        case 'Z':
          {
            CommandData d;
            d.type = (command == 'w' || command == 'W') ? WAKEUP : POWEROFF;


            char * pch;
            pch = strtok (&message[1], ",");
            String split[1];
            int i = 0;
            while (pch != NULL && i < 1)
            {
              split[i] = String(pch);
              pch = strtok (NULL, ",");
              i++;
            }

            d.value1.intValue = split[0].toInt(); //group id
            d.value2.intValue = (command == 'W' || command == 'Z'); //public with capital
            sendCommand(d);

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
            pch = strtok (&message[1], ",");
            String split[13];
            int i = 0;
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
