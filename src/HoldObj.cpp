/*
  HoldObj - Simple class used by EasierButton.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#include "Arduino.h"
#include "HoldObj.h"
#include <vector>

// constructor
HoldObj::HoldObj(unsigned long holdDuration, callback cb)
{
  duration = holdDuration;
  _cb = cb;
  called = false;
}

// Public Members

void HoldObj::reset() {
  called = false;
}

void HoldObj::trigger() {
  if (called) return;

  called = true;
  _cb();
}
