/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#ifndef EasierButton_h
#define EasierButton_h

#include <vector>
#include "Arduino.h"
#include "HoldObj.h"
#include <EasyButton.h>
#include "EasyMultiClick.h"

struct EasyTimer {
  unsigned long sinceLastPress;
  unsigned long sinceLastRelease;
};

typedef void (*voidCallback) ();
typedef void (*voidCallbackTimer) (EasyTimer);

struct DelayedCb {
  unsigned long duration;
  voidCallback cb;
  bool strict;
};

typedef std::vector<HoldObj> hold_obj_vector;
typedef std::vector<DelayedCb> delay_vector;

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
    bool begin(unsigned long duration);
    unsigned long begin(unsigned long duration, bool returnElapsed);

    void update();

    bool pressedAtBoot();
    unsigned long heldAtBoot();
    bool waitForClick(unsigned long duration);
    bool heldFor(unsigned long duration);
    bool heldFor(unsigned long duration, bool boot);
    unsigned long getHoldDuration(unsigned long duration);
    unsigned long getHoldDuration(unsigned long duration, bool boot);

    void setOnPressed(voidCallback cb);
    void setOnPressed(voidCallbackTimer cb);
    void setOnHold(unsigned long duration, voidCallback cb);
    void setOnHold(unsigned long duration, voidCallback cb, bool strict);
    
    void setOnReleased(voidCallback cb);
    void setOnReleased(voidCallbackTimer cb);
    void setOnReleasedAfter(unsigned long duration, voidCallback cb);
    void setOnReleasedAfter(unsigned long duration, voidCallback cb, bool strict);
    
    void setMultiClickTimeout(unsigned long timeout);
    void setOnSingleClick(voidCallback cb);
    void setOnDoubleClick(voidCallback cb);
    void setOnTripleClick(voidCallback cb);

    void defineMaxClickDuration(unsigned long duration);
  private:
    bool _begun;
    bool _lastState;
    bool _pressedAtBoot;
    bool _firstClickInLoop;
    bool _ignoreNextRelease;
    bool _ignoreTimedOutClicks;
    unsigned long _heldAtBoot;
    unsigned long _maxClickDuration;

    void _setup();
    void _handleCallOverdueHolds();
    void _handlePressed(unsigned long &now);
    void _handleReleased(unsigned long &now);
    void _handleMultiClick(unsigned long &now);
    void _handleCallOnReleasedAfters(unsigned long &pressDuration);
    
    void _checkBootPress();
    bool _checkBootHold(unsigned long duration);
    unsigned long _checkBootHold(unsigned long duration, bool returnElapsed);

    unsigned long _lastPress;
    unsigned long _lastRelease;
    unsigned long _multiClickTimeout;
    int _cachedClicks;

    voidCallback _onPressed;
    voidCallbackTimer _onPressedTimer;

    voidCallback _onReleased;
    voidCallbackTimer _onReleasedTimer;

    bool _multiClickSet;
    EasyMultiClick _multiClick;
    voidCallback _onSingleClick;
    voidCallback _onDoubleClick;
    voidCallback _onTripleClick;

    hold_obj_vector _onHoldObjs;
    delay_vector _onReleasedObjs;
};

#endif
