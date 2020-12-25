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
  _heldAtBoot = false;
  _pressedAtBoot = false;
  _multiClickSet = false;

  unsigned long now = millis();
  _lastPress = now;
  _lastRelease = now;
}

void EasierButton::_handlePressed(unsigned long &now)
{ 
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

  if (_multiClickSet) {
    _multiClick.inc(now);
  }
}

void EasierButton::_handleReleased(unsigned long &now)
{
  _lastState = false;
  unsigned long pressDuration = now - _lastPress;
  unsigned long sinceLastRelease = now - _lastRelease;
  _lastRelease = now;

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

  // button released, so mark all holds as not called
  for(HoldObj &obj : _onHoldObjs) obj.reset();
  _handleCallOnReleasedAfters(pressDuration);
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
  }
}

void EasierButton::_checkBootPress()
{
  if (easyButton.isPressed())
  {
    _pressedAtBoot = true;
  }
}

unsigned long EasierButton::_checkBootHold(int duration, bool returnElapsed)
{
  unsigned long elapsed = 0;
  unsigned long start = millis();
  bool held = easyButton.isPressed();
  _pressedAtBoot = held;

  while (held && (elapsed < (unsigned long)duration)) {
    easyButton.read();
    held = !easyButton.wasReleased();
    _heldAtBoot = held;

    if (!held) break;

    delay(10); // so we don't go crazy
    elapsed = millis() - start;
  }

  return elapsed;
}

bool EasierButton::_checkBootHold(int duration)
{
  unsigned long elapsed = _checkBootHold(duration, true);

  return elapsed >= (unsigned long)duration;
}

void EasierButton::_handleCallOverdueHolds()
{
  // check each of the hold callbacks and call if they are past due
  for(HoldObj &obj : _onHoldObjs) {
    if (obj.called) continue; // only call once

    if (easyButton.pressedFor(obj.duration))
    {
      obj.trigger();
    }
  }
}

void EasierButton::_handleMultiClick(unsigned long &now)
{
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

bool EasierButton::begin(int duration) {
  if (_begun) return false;

  _begun = true;
  easyButton.begin();

  return _checkBootHold(duration);
}

unsigned long EasierButton::begin(int duration, bool returnElapsed) {
  if (_begun) return 0;

  _begun = true;
  easyButton.begin();

  return _checkBootHold(duration, returnElapsed);
}

void EasierButton::update()
{
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

  if (easyButton.isPressed()) {
    _handleCallOverdueHolds();
  }

  // must be called each loop
  _handleMultiClick(now);
}

bool EasierButton::pressedAtBoot() {
  return _pressedAtBoot;
}

bool EasierButton::heldAtBoot() {
  return _heldAtBoot;
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
  HoldObj obj(duration, cb);
  _onHoldObjs.push_back(obj);
}

void EasierButton::setOnReleasedAfter(unsigned long duration, callback cb) {
  DelayedCb obj = {
    cb = cb,
    duration = duration,
  };
  _onReleasedObjs.push_back(obj);
}
