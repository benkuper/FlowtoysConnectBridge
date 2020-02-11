#pragma once
#include "FastLED.h"

#define DATA_PIN 25
#define CLOCK_PIN 26
#define NUM_LEDS 4 //2 leds on new bridge

class LedManager
{
public:
    CRGB leds[NUM_LEDS];

    void init()
    {
      LEDS.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
      LEDS.setBrightness(80);
    }

    void setLed(int id, CRGB color)
    {
      if (id < 0 || id >= NUM_LEDS) return;
      leds[id] = color;
      LEDS.show();
    }

    void setAll(CRGB color)
    {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = color;
      LEDS.show();
    }
};
