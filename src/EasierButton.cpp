/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

#include "Arduino.h"
#include <EasyButton.h>
#include "EasierButton.h"
#include <vector>
#include "HoldObj.h"

void debug(int val) {
  #ifdef EASIER_DEBUG
    Serial.print(val);
  #endif
}

void debugln(int val) {
  #ifdef EASIER_DEBUG
    Serial.println(val);
  #endif
}

void debug(String val) {
  #ifdef EASIER_DEBUG
    Serial.print(val);
  #endif
}

void debugln(String val) {
  #ifdef EASIER_DEBUG
    Serial.println(val);
  #endif
}

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
  unsigned long releaseToPress = now - _lastRelease;
  _lastPress = now;

  debugln(F("Button was pressed."));
  debug(F("It had been "));
  debug(pressToPress);
  debug(F(" ms since the button was last pressed, and "));
  debug(releaseToPress);
  debugln(F(" ms since the button was last released."));

  if (_onPressedLong)
  {
    _onPressedLong(pressToPress);
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
  _lastRelease = now;

  debugln(F("Button was released."));
  debug(F("The button was pressed for "));
  debug(pressDuration);
  debugln(F(" ms."));

  if (_onReleasedLong)
  {
    _onReleasedLong(pressDuration);
  }
  else if (_onReleased)
  {
    _onReleased();
  }
}

void EasierButton::_checkAtBoot()
{
  if (easyButton.isPressed())
  {
    debugln("Button PRESSED at boot.");
    _pressedAtBoot = true;
  }
  else
  {
    debugln("Button not pressed at boot.");
  }
}

// Public Members

void EasierButton::begin() {
  easyButton.begin();
  _checkAtBoot();
}

bool EasierButton::blockBoot(int duration)
{
  bool held = false;
  if (easyButton.isPressed())
  {
    _pressedAtBoot = true;
    delay(duration);
    easyButton.read();
    held = !easyButton.wasReleased();
  }

  _heldAtBoot = held;

  return held;
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

void EasierButton::setOnPressed(voidCallbackLong cb) {
  _onPressedLong = cb;
}

void EasierButton::setOnReleased(voidCallback cb) {
  _onReleased = cb;
}

void EasierButton::setOnReleased(voidCallbackLong cb) {
  _onReleasedLong = cb;
}

void EasierButton::setOnHold(unsigned long duration, callback cb) {
  HoldObj obj(duration, cb);
  onHoldObjs.push_back(obj);
}
