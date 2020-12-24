/*
  EasyMultiClick - Used by EasierButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef EasyMultiClick_h
#define EasyMultiClick_h

#include "Arduino.h"

class EasyMultiClick
{
  public:
    EasyMultiClick();
    void reset();
    uint8_t numClicks();
    bool overdue();
    void inc(unsigned long now);
    void setTimeout(unsigned long timeout);
  private:
    uint8_t _numClicks;
    unsigned long _firstClick;
    unsigned long _multiClickTimeout;
};

#endif
