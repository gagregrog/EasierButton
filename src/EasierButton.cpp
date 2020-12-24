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

  unsigned long now = millis();
  _lastPress = now;
  _lastRelease = now;
}

void EasierButton::_handlePressed()
{
  if (_lastState) return;
  
  _lastState = true;
  unsigned long now = millis();
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

void EasierButton::_handleReleased()
{
  if (!_lastState) return;

  _lastState = false;
  unsigned long now = millis();
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
  easyButton.read();

  for(HoldObj &obj : onHoldObjs) {
    if (obj.called) continue;

    if (_lastState && easyButton.pressedFor(obj.duration))
    {
      obj.trigger();
    }
  }

  if (easyButton.wasReleased())
  {
    for(HoldObj &obj : onHoldObjs)
    {
      obj.reset();
    }
    
    _handleReleased();
  }

  if (easyButton.wasPressed())
  {
    _handlePressed();
  }
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

void EasierButton::setOnHold(unsigned long duration, callback cb) {
  HoldObj obj(duration, cb);
  onHoldObjs.push_back(obj);
}
