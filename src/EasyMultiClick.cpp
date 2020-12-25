/*
  EasyMultiClick - Simple class used by EasierButton.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#include "Arduino.h"
#include "EasyMultiClick.h"

// constructor
EasyMultiClick::EasyMultiClick()
{
  reset();
  _multiClickTimeout = 375;
}

// Public Members

void EasyMultiClick::reset() {
  _numClicks = 0;
  _firstClick = 0;
}

void EasyMultiClick::setTimeout(unsigned long timeout) {
  _multiClickTimeout = timeout;
}

unsigned long EasyMultiClick::getTimeout() {
  return _multiClickTimeout;
}

bool EasyMultiClick::overdue() {
  return (millis() - _firstClick >= _multiClickTimeout);
}

bool EasyMultiClick::overdue(unsigned long now) {
  return (now - _firstClick >= _multiClickTimeout);
}

void EasyMultiClick::inc(unsigned long now) {
  if (!_numClicks++) {
    _firstClick = now;
  }
}

uint8_t EasyMultiClick::numClicks() {
  return _numClicks;
}
