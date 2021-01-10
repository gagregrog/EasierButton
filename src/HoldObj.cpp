/*
  HoldObj - Simple class used by EasierButton.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#include "Arduino.h"
#include "HoldObj.h"
#include <vector>

// constructor
HoldObj::HoldObj(unsigned long holdDuration, voidCallback cb, bool strict)
{
  duration = holdDuration;
  _cb = cb;
  called = false;
  _strict = strict;
}

// Public Members

void HoldObj::reset() {
  called = false;
}

bool HoldObj::trigger() {
  if (called) return false;

  called = true;
  _cb();

  return _strict;
}
