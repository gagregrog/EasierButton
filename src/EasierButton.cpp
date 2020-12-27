/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#include <vector>
#include "Arduino.h"
#include "HoldObj.h"
#include <EasyButton.h>
#include "EasierButton.h"
#include "EasyMultiClick.h"

// constructor
EasierButton::EasierButton(uint8_t pin, uint32_t debounce, bool pullUp, bool active_low): easyButton(pin, debounce, pullUp, active_low)
{
  _setup();
}

EasierButton::EasierButton(uint8_t pin, uint32_t debounce, bool pullUp): easyButton(pin, debounce, pullUp, pullUp)
{
  _setup();
}

EasierButton::EasierButton(uint8_t pin, uint32_t debounce): easyButton(pin, debounce, true, true)
{
  _setup();
}

EasierButton::EasierButton(uint8_t pin, bool pullUp): easyButton(pin, 35, pullUp, pullUp)
{
  _setup();
}

EasierButton::EasierButton(uint8_t pin): easyButton(pin, 35, true, true)
{
  _setup();
}

// Private Members
void EasierButton::_setup()
{
  _begun = false;
  _lastState = false;
  _pressedAtBoot = false;
  _multiClickSet = false;
  _firstClickInLoop = false;
  _ignoreNextRelease = false;
  _ignoreTimedOutClicks = false;

  unsigned long now = millis();
  _heldAtBoot = 0;
  _lastPress = now;
  _lastRelease = now;
  _maxClickDuration = 400;
}

void EasierButton::_handlePressed(unsigned long &now)
{ 
  if (!_firstClickInLoop) {
    _firstClickInLoop = true;
  }

  _lastState = true;
  unsigned long pressToPress = now - _lastPress;
  _lastPress = now;
  
  unsigned long releaseToPress = now - _lastRelease;

  if (_onPressedTimer)
  {
    EasyTimer time = {
      .sinceLastPress = pressToPress,
      .sinceLastRelease = releaseToPress,
    };
    _onPressedTimer(time);
  }
  else if (_onPressed)
  {
    _onPressed();
  }
}

void EasierButton::_handleReleased(unsigned long &now)
{
  _lastState = false;

  if (!_firstClickInLoop) {
    _lastRelease = now;
    _lastPress = now;
    return;
  }

  unsigned long pressDuration = now - _lastPress;
  unsigned long sinceLastRelease = now - _lastRelease;
  _lastRelease = now;

  if (_multiClickSet) {
    if (pressDuration < _maxClickDuration) {
      _multiClick.inc(now);
    } else {
      // if the button is held longer than 500ms
      // don't consider it a click
      _multiClick.reset();
    }
  }

  if (!_ignoreNextRelease) {
    if (_onReleasedTimer)
    {
      EasyTimer time = {
        .sinceLastPress = pressDuration,
        .sinceLastRelease = sinceLastRelease,
      };

      _onReleasedTimer(time);
    }
    else if (_onReleased)
    {
      _onReleased();
    }

    // TODO: consider moving this above the release callback
    // and canceling the release callbacks if strict 
    // release afters are called
    _handleCallOnReleasedAfters(pressDuration);
  }

  // button released, so mark all holds as not called
  for(HoldObj &obj : _onHoldObjs) obj.reset();

  _ignoreNextRelease = false;
}

void EasierButton::_handleCallOnReleasedAfters(unsigned long &pressDuration)
{
  // iterate over the onReleasedAfter cbs and see find the one with the largest duration that is less then the amount of time the button was pressed for
  int longestHeldIdx = -1;

  for(unsigned int i = 0; i < _onReleasedObjs.size(); i++)
  {
    DelayedCb &obj = _onReleasedObjs[i];
    if (
      (pressDuration >= obj.duration) &&
      (longestHeldIdx < 0 || (_onReleasedObjs[longestHeldIdx].duration < obj.duration))
    )
    {
      longestHeldIdx = i;
    }
  }

  // if we found an onReleaseAfter cb that was longer than the length of the button press, call it now
  if (longestHeldIdx > -1) {
    _onReleasedObjs[longestHeldIdx].cb();

    // if it is strict, cancel any future timed out multi clicks
    if (_onReleasedObjs[longestHeldIdx].strict) {
      _ignoreTimedOutClicks = true;
    }
  }
}

void EasierButton::_checkBootPress()
{
  if (easyButton.isPressed())
  {
    _pressedAtBoot = true;
  }
}

unsigned long EasierButton::_checkBootHold(unsigned long duration, bool returnElapsed)
{
  unsigned long elapsed = getHoldDuration(duration, true);

  return returnElapsed ? elapsed : (elapsed >= (unsigned long)duration);
}

bool EasierButton::_checkBootHold(unsigned long duration)
{
  return _checkBootHold(duration, false);
}

void EasierButton::_handleCallOverdueHolds()
{
  // check each of the hold callbacks and call if they are past due
  for(HoldObj &obj : _onHoldObjs) {
    if (obj.called) continue; // only call once

    if (easyButton.pressedFor(obj.duration))
    {
      bool strict = obj.trigger();
      // if this callback is strict, ignore future button releases
      if (strict) {
        _ignoreNextRelease = true;
      }
    }
  }
}

void EasierButton::_handleMultiClick(unsigned long &now)
{
  // if we are in this realm then we are holding the button, not pressing it
  if (_ignoreTimedOutClicks) {
    _multiClick.reset();
    _ignoreTimedOutClicks = false;

    return;
  };

  int numClicks = _multiClick.numClicks();
  if (_multiClickSet && numClicks) {
    if (_onTripleClick) {
      if (numClicks == 3) {
        _onTripleClick();
        _multiClick.reset();
        return;
      }
    } else if (_onDoubleClick) {
      if (numClicks == 2) {
        _onDoubleClick();
        _multiClick.reset();
        return;
      }
    } else if (_onSingleClick) {
      _onDoubleClick();
      _multiClick.reset();
      return;
    }
    
    if (_multiClick.overdue(now)) {
      _multiClick.reset();
      if (_onDoubleClick && numClicks == 2) {
        _onDoubleClick();
      } else if (_onSingleClick) {
        _onSingleClick();
      }
    }
  }
}

// Public Members

void EasierButton::begin() {
  if (_begun) return;

  _begun = true;
  easyButton.begin();
  _checkBootPress();
}

bool EasierButton::begin(unsigned long duration) {
  if (_begun) return false;

  _begun = true;
  easyButton.begin();

  return _checkBootHold(duration);
}

unsigned long EasierButton::begin(unsigned long duration, bool returnElapsed) {
  if (_begun) return 0;

  _begun = true;
  easyButton.begin();

  return _checkBootHold(duration, returnElapsed);
}

void EasierButton::update()
{
  bool noFirstClick = !_firstClickInLoop;

  unsigned long now = millis();
  easyButton.read();

  if (easyButton.wasReleased())
  {
    _handleReleased(now);
  }

  if (easyButton.wasPressed())
  {
    _handlePressed(now);
  }

  // let's not respond to releases/holds from boot
  if (noFirstClick) return;

  if (easyButton.isPressed()) {
    _handleCallOverdueHolds();
  }

  // must be called each loop
  _handleMultiClick(now);
}

bool EasierButton::pressedAtBoot() {
  return _pressedAtBoot;
}

unsigned long EasierButton::heldAtBoot() {
  return _heldAtBoot;
}

void EasierButton::defineMaxClickDuration(unsigned long duration) {
  _maxClickDuration = duration;
}

void EasierButton::setOnPressed(voidCallback cb) {
  _onPressed = cb;
}

void EasierButton::setOnPressed(voidCallbackTimer cb) {
  _onPressedTimer = cb;
}

void EasierButton::setOnReleased(voidCallback cb) {
  _onReleased = cb;
}

void EasierButton::setOnReleased(voidCallbackTimer cb) {
  _onReleasedTimer = cb;
}

void EasierButton::setOnSingleClick(voidCallback cb) {
  _onSingleClick = cb;
  _multiClickSet = true;
}

void EasierButton::setOnDoubleClick(voidCallback cb) {
  _onDoubleClick = cb;
  _multiClickSet = true;
}

void EasierButton::setOnTripleClick(voidCallback cb) {
  _onTripleClick = cb;
  _multiClickSet = true;

  if (_multiClick.getTimeout() == 375) {
    _multiClick.setTimeout(600);
  }
}

void EasierButton::setMultiClickTimeout(unsigned long timeout) {
  _multiClick.setTimeout(timeout);
}

void EasierButton::setOnHold(unsigned long duration, callback cb) {
  setOnHold(duration, cb, true);
}

void EasierButton::setOnHold(unsigned long duration, callback cb, bool strict) {
  HoldObj obj(duration, cb, strict);
  _onHoldObjs.push_back(obj);
}

void EasierButton::setOnReleasedAfter(unsigned long duration, callback cb) {
  setOnReleasedAfter(duration, cb, true);
}

void EasierButton::setOnReleasedAfter(unsigned long duration, callback cb, bool strict) {
  DelayedCb obj = {
    duration,
    cb,
    strict,
  };

  _onReleasedObjs.push_back(obj);
}

bool EasierButton::waitForClick(unsigned long duration)
{
  unsigned long start = millis();
  
  // if we start in a pressed state and never release, don't consider that a click
  while (easyButton.isPressed()) {
    delay(10);
    if (millis() - start >= duration) return false;
    easyButton.read();
  }

  // if we made it here, we've found an unpressed state
  while (easyButton.isReleased()) {
    delay(10);
    if (millis() - start >= duration) return false;
    easyButton.read();
  }

  // if we exit the loop above then a button press occurred
  return true;
}

unsigned long EasierButton::getHoldDuration(unsigned long duration)
{
  return getHoldDuration(duration, false);
}

unsigned long EasierButton::getHoldDuration(unsigned long duration,  bool boot)
{
  unsigned long elapsed = 0;
  unsigned long start = millis();
  bool held = easyButton.isPressed();

  while (held && (elapsed < (unsigned long)duration)) {
    easyButton.read();
    held = !easyButton.wasReleased();

    if (!held) break;

    delay(10); // so we don't go crazy
    elapsed = millis() - start;
  }

  if (boot) {
    _heldAtBoot += elapsed;
    if (elapsed) {
      _pressedAtBoot = true;
    }
  }

  return elapsed;
}

bool EasierButton::heldFor(unsigned long duration, bool boot) {
  unsigned long elapsed = getHoldDuration(duration, boot);

  return elapsed >= duration;
}

bool EasierButton::heldFor(unsigned long duration) {
  return heldFor(duration, false);
}
