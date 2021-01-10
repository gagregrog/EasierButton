/*
  HoldObj - Used by EasierButton.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef HoldObj_h
#define HoldObj_h

#include "Arduino.h"
#include <vector>

typedef void (*voidCallback) ();

class HoldObj
{
  public:
    HoldObj(unsigned long holdDuration, voidCallback cb, bool strict);
    bool called;
    void reset();
    bool trigger();
    unsigned long duration;
  private:
    voidCallback _cb;
    bool _strict;
};

#endif
