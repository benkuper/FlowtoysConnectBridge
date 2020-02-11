#pragma once
#include "CommandProvider.h"

//2 buttons wired on pin 22 and 23 (hardware pull up), no need to debounce, just a digital read

#define NUM_BUTTONS 2

#define LONGPRESS_TIME 500 //more than 500ms is long press
#define VERYLONGPRESS_TIME 1500
#define SHORTPRESS_TIME 500 //less than 500ms is short press
#define MULTIPRESS_TIME 300 //each new press shorter than 500ms after the previous one will increase the multiclick
 

class ButtonManager :
  public CommandProvider
{
  public:
    ButtonManager() : CommandProvider("Button") {}
    ~ButtonManager() {}

    const int buttonPins[NUM_BUTTONS]{22,23};
    
    bool pressed[NUM_BUTTONS];
    bool longPress[NUM_BUTTONS];
    bool veryLongPress[NUM_BUTTONS];
    long timeAtPress[NUM_BUTTONS];
    int multipressCount[NUM_BUTTONS];

    void init()
    {
      for (int i = 0; i < NUM_BUTTONS; i++)
      {
         pinMode(buttonPins[i], INPUT);
         pressed[i] = false;
        longPress[NUM_BUTTONS] = false;
        veryLongPress[NUM_BUTTONS] = false;
        timeAtPress[NUM_BUTTONS] = false;
        multipressCount[NUM_BUTTONS] = false;
      }
    }

    void update()
    {
      for(int i=0;i<NUM_BUTTONS;i++)
      {
        int val = !digitalRead(buttonPins[i]);
        setButton(i, val);
      }
    }

    void setButton(int id, bool value)
    {
      if (pressed[id] != value) //button state changed
      {
        pressed[id] = value;
        longPress[id] = false;
        veryLongPress[id] = false;
        
        if (pressed[id])
        {
          timeAtPress[id] = millis();

          handlePressed(id, true);

          multipressCount[id] ++;
          if (multipressCount[id] > 1) handleMultiPress(id, multipressCount[id]);
        } else
        {
          if (millis() < timeAtPress[id] + SHORTPRESS_TIME) handleShortPress(id);
          handlePressed(id, false);
        }
      } else
      {
        if (pressed[id])
        {
          if (!longPress[id] && millis() > timeAtPress[id] + LONGPRESS_TIME)
          {
            longPress[id] = true;
            handleLongPress(id);
          }

          if (!veryLongPress[id] && millis() > timeAtPress[id] + VERYLONGPRESS_TIME)
          {
            veryLongPress[id] = true;
            handleVeryLongPress(id);
          }
        }

        if (millis() > timeAtPress[id] + MULTIPRESS_TIME) multipressCount[id] = 0;
      }
    }

    // EVENT HANDLING / COMMAND SENDNG
    void handlePressed(int id, bool value)
    {
      Serial.println("Pressed " + String(id));
      switch (id)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;
      }
    }

    void handleShortPress(int id)
    {
      Serial.println("Short press " + String(id));
      switch (id)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;
      }
    }

    void handleLongPress(int id)
    {
      Serial.println("Long press " + String(id));
      switch (id)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;
      }
    }

    void handleVeryLongPress(int id)
    {
      Serial.println("Very long press " + String(id));
      switch (id)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;
      }
    }

    void handleMultiPress(int id, int count)
    {
      DBG("Multi press " + String(id) + " : " + String(count));
      switch (id)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;
      }
    }

};
