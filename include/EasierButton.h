/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef EasierButton_h
#define EasierButton_h

#include "Arduino.h"
#include <EasyButton.h>

// typedef void (*voidCallback) ();
// typedef void (*voidCallbackStr) (String);
// typedef String (*stringCallback) ();
// typedef String (*stringCallbackStr) (String);

class EasierButton
{
  public:
    EasierButton();

  private:
    void _flagReboot();
};

#endif
