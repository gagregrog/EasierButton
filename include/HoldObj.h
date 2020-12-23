/*
  HoldObj - Used by EasierButton.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef HoldObj_h
#define HoldObj_h

#include "Arduino.h"
#include <vector>

typedef std::function<void()> callback;

class HoldObj
{
  public:
    HoldObj(unsigned long holdDuration, callback cb);
    bool called;
    unsigned long duration;
    void reset();
    void trigger();
  private:
    callback _cb;
};

#endif
