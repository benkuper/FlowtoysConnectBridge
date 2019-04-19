#pragma once
#include "CommandProvider.h"
#define BUTTON_PIN 39
#define NUM_BUTTONS 4
#define NUM_BT_VALUES 16

#define DEBOUNCE_COUNT 100

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
    
    int buttonVals[NUM_BT_VALUES] {4096, 3955, 3520, 3240, 2980, 2837, 2707, 2608, 2414, 2346, 2281, 2222, 2152, 2107, 2057, 2015 };

    int buttonCounters[NUM_BUTTONS]; //debounce

    bool pressed[NUM_BUTTONS];
    bool longPress[NUM_BUTTONS];
    bool veryLongPress[NUM_BUTTONS];
    long timeAtPress[NUM_BUTTONS];
    int multipressCount[NUM_BUTTONS];

    void init()
    {
      for(int i=0;i<NUM_BT_VALUES;i++)
      {
         buttonVals[i] = Config::instance->getButtonStateVal(i);
         DBG("Set button state "+String(i)+" value : "+String(buttonVals[i]));
      }
      
      pinMode(BUTTON_PIN, INPUT);
      for (int i = 0; i < NUM_BUTTONS; i++)
      {
        buttonCounters[i] = 0;
        pressed[i] = false;
        longPress[NUM_BUTTONS] = false;
        veryLongPress[NUM_BUTTONS] = false;
        timeAtPress[NUM_BUTTONS] = false;
        multipressCount[NUM_BUTTONS] = false;
      }
    }

    void update()
    {
      int val = analogRead(BUTTON_PIN);

      for (int i = 0; i < NUM_BT_VALUES; i++)
      {
        if (abs(val - buttonVals[i]) < 30)
        {
          //Serial.print(String(val) + " / " + String(buttonVals[i]) + " : ");

          for (int j = 0; j < NUM_BUTTONS; j++)
          {
            bool v = (i >> j) & 1;
            buttonCounters[j] += v ? 1 : -1;
            buttonCounters[j] = min(max(buttonCounters[j], 0), DEBOUNCE_COUNT);
            if (buttonCounters[j] == DEBOUNCE_COUNT) setButton(j, true);
            else if (buttonCounters[j] == 0) setButton(j, false);
            //Serial.print(String(buttonCounters[j])+" ");
          }
          //Serial.println("");
          break;
        }
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


    void launchCalibration()
    {
      DBG("Launch Calibration");
      for(int i=0;i<NUM_BT_VALUES;i++)
      {
         DBG("["+String(i)+"] Press "+String(i & 1)+" "+String((i >> 1) & 1)+" "+String((i >> 2) & 1)+" "+String((i >> 3) & 1));
         
         delay(2000);

         DBG("Calibrating...");
         long average = 0;
         for(int j=0;j<200;j++)
         {
            average += analogRead(BUTTON_PIN);
         }

         average /= 200;

         DBG("Average is "+String(average));
           
         Config::instance->setButtonStateVal(i, average);
      }

      for(int i=0;i<NUM_BT_VALUES;i++)
      {
         buttonVals[i] = Config::instance->getButtonStateVal(i);
         DBG("Set button state "+String(i)+" value : "+String(buttonVals[i]));
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
      Serial.println("Multi press " + String(id) + " : " + String(count));
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
