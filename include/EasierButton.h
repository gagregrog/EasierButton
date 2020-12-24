/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef EasierButton_h
#define EasierButton_h

#include <vector>
#include "Arduino.h"
#include <EasyButton.h>
#include "HoldObj.h"

struct EasyTimer {
  unsigned long sinceLastPress;
  unsigned long sinceLastRelease;
};

typedef void (*voidCallback) ();
typedef void (*voidCallbackTimer) (EasyTimer);

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
    bool begin(int duration);
    unsigned long begin(int duration, bool returnElapsed);

    void update();

    bool heldAtBoot();
    bool pressedAtBoot();

    void setOnPressed(voidCallback cb);
    void setOnPressed(voidCallbackTimer cb);
    void setOnReleased(voidCallback cb);
    void setOnReleased(voidCallbackTimer cb);
    void setOnHold(unsigned long duration, callback cb);

  private:
    bool _begun;
    bool _lastState;
    bool _heldAtBoot;
    bool _pressedAtBoot;

    void _setup();
    void _handlePressed();
    void _handleReleased();
    
    void _checkBootPress();
    bool _checkBootHold(int duration);
    unsigned long _checkBootHold(int duration, bool returnElapsed);

    unsigned long _lastPress;
    unsigned long _lastRelease;

    voidCallback _onPressed;
    voidCallbackTimer _onPressedTimer;

    voidCallback _onReleased;
    voidCallbackTimer _onReleasedTimer;

    hold_obj_vector onHoldObjs;
};

#endif