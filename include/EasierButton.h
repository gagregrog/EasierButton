/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef EasierButton_h
#define EasierButton_h

#include "Arduino.h"
#include <EasyButton.h>
#include <vector>
#include "HoldObj.h"

typedef void (*voidCallback) ();
typedef void (*voidCallbackLong) (unsigned long);

typedef std::function<void()> callback;
typedef std::vector<HoldObj> hold_obj_vector;

class EasierButton
{
  public:
    EasyButton easyButton;
    EasierButton(uint8_t pin);
    EasierButton(uint8_t pin, bool pullUp);
    EasierButton(uint8_t pin, uint32_t debounce);
    EasierButton(uint8_t pin, uint32_t debounce, bool pullUp);
    EasierButton(uint8_t pin, uint32_t debounce, bool pullUp, bool active_low);

    void begin();
    void update();

    bool heldAtBoot();
    bool pressedAtBoot();
    bool blockBoot(int duration);

    void setOnPressed(voidCallback cb);
    void setOnPressed(voidCallbackLong cb);
    void setOnReleased(voidCallback cb);
    void setOnReleased(voidCallbackLong cb);
    void setOnHold(unsigned long duration, callback cb);

  private:
    bool _lastState;
    bool _pressedAtBoot;
    bool _heldAtBoot;

    void _setup();
    void _handlePressed();
    void _handleReleased();
    void _checkAtBoot();

    unsigned long _lastPress;
    unsigned long _lastRelease;

    voidCallback _onPressed;
    voidCallbackLong _onPressedLong;

    voidCallback _onReleased;
    voidCallbackLong _onReleasedLong;

    hold_obj_vector onHoldObjs;
};

#endif
