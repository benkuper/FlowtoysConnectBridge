#pragma once
#include "CommandProvider.h"

//2 buttons wired on pin 22 and 23 (hardware pull up), no need to debounce, just a digital read

#define NUM_BUTTONS 2

#define LONGPRESS_TIME 500 //more than 500ms is long press
#define VERYLONGPRESS_TIME 1500
#define SHORTPRESS_TIME 500 //less than 500ms is short press
#define MULTIPRESS_TIME 300 //each new press shorter than 500ms after the previous one will increase the multiclick


class ButtonManager
{
  public:
    ButtonManager()
    {
      setEventCallbacks(&ButtonManager::buttonPressDefaultEvent,
                        &ButtonManager::buttonDefaultEvent,
                        &ButtonManager::buttonDefaultEvent,
                        &ButtonManager::buttonDefaultEvent,
                        &ButtonManager::buttonMultiPressDefaultEvent
      );  
    }
    
    ~ButtonManager() {}

    const int buttonPins[NUM_BUTTONS] {23,22};

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
      for (int i = 0; i < NUM_BUTTONS; i++)
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
      onButtonPress(id, value);
    }

    void handleShortPress(int id)
    {
      onButtonShortPress(id);
    }

    void handleLongPress(int id)
    {
      onButtonLongPress(id);
    }

    void handleVeryLongPress(int id)
    {
      onButtonVeryLongPress(id);
    }

    void handleMultiPress(int id, int count)
    {
      onButtonMultiPress(id, count);
    }


    typedef void(*buttonValueEvent)(int,bool);
    void (*onButtonPress) (int, bool);
    typedef void(*buttonEvent)(int);
    void (*onButtonShortPress) (int);
    void (*onButtonLongPress) (int);
    void (*onButtonVeryLongPress) (int);
    typedef void(*multiPressEvent)(int,int);
    void (*onButtonMultiPress) (int, int);

    void setEventCallbacks (buttonValueEvent pressFunc, 
                    buttonEvent shortPressFunc, 
                    buttonEvent longPressFunc, 
                    buttonEvent veryLongPressFunc, 
                    multiPressEvent multiPressFunc
                ) {
      onButtonPress = pressFunc;
      onButtonShortPress = shortPressFunc;
      onButtonLongPress = longPressFunc;
      onButtonVeryLongPress = veryLongPressFunc;
      onButtonMultiPress = multiPressFunc;
    }

    static void buttonPressDefaultEvent(int id, bool value){}
    static void buttonDefaultEvent(int id){}
    static void buttonMultiPressDefaultEvent(int id, int count){}
};
