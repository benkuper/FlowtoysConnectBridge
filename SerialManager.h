#pragma once

#include "CommandProvider.h"

#if SERIAL_DEBUG
#define DBG(msg) Serial.println(msg)
#else
#define DBG()
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
    char buffer[32];
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
        if (c == 255 || c == '\n')
        {
          parseMessage();
          memset(buffer, 0, 32);
          bufferIndex = 0;
        } else
        {
          if (bufferIndex < 32) buffer[bufferIndex] = c;
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

        case 'w':
          {
            DBG(String("Buffer 1 is ") + (int)buffer[1]);
            if (buffer[1] == 0 || buffer[1] == '0') sendCommand(POWEROFF); //either with bytes or char
            else sendCommand(WAKEUP);
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

        case 'n':
          {
            CommandData d;
            d.type = SET_WIFI_CREDENTIALS;
            d.value1.stringValue = "tatouille";
            d.value2.stringValue = "pirouette";
            sendCommand(d);
          }
          break;
      }
    }

};

SerialManager * SerialManager::instance = nullptr;
