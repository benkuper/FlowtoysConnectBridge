#pragma once
#include "CommandProvider.h"

class ButtonManager :
  public CommandProvider
{
public:
  ButtonManager() : CommandProvider("Button") {}
  ~ButtonManager() {}
 
  unsigned long pushed = 0;
  byte fast_push = 0;
  
  void init()
  {
    pinMode(0, INPUT); // use boot button as user button
  }
  
  void update()
  {
    
  }
};

