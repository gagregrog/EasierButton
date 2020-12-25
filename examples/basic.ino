/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
*/

/*
Button Setup ----------------------------------------------------------------

To use with internal pullup resistor (Default setting):
    connect one pin of a button high (3.3V or 5V) via a 10k ohm resistor
    connect the other pin to ground 
    connect the high side of the button to your desired pin

To use without internal pullup resistor [EasierButton myBtn(btn_pin, false)]:
    connect one pin of a button high (3.3V or 5V)
    connect the other pin to ground via a 10k ohm resistor
    connect the low side of the button to pin your desired pin
*/

#include <Arduino.h>
#include <EasierButton.h>

uint8_t btn_pin = D0;

// defaults to 35ms debounce, internal_pullup = true, active_low = true
EasierButton myBtn(btn_pin);

// defaults to internal_pullup = true, active_low = true
// uint32_t debounce = 55;
// EasierButton myBtn(btn_pin, debounce);

// defaults to 35ms debounce, sets active_low = pullup
// bool pullup = false;
// EasierButton myBtn(btn_pin, pullup);

// sets active_low = pullup
// uint32_t debounce = 55;
// bool pullup = true;
// EasierButton myBtn(btn_pin, debounce, pullup);

// or set everything manually
// uint32_t debounce = 55;
// bool pullup = false;
// bool invert = true;
// EasierButton myBtn(btn_pin, debounce, pullup, invert);

// or void onPress();
// called EVERY time the button is pressed
// except when held during reboot
void onPress(EasyTimer time) {
  Serial.print(F("You pressed the button! It was last pressed "));
  Serial.print(time.sinceLastPress);
  Serial.print(F("ms ago and was last released "));
  Serial.print(time.sinceLastRelease);
  Serial.println(F("ms ago."));
}

// or void onRelease();
// called EVERY time the button is released
// except when released immediately after a reboot hold
void onRelease(EasyTimer time) {
  Serial.print(F("You released the button! It was held for "));
  Serial.print(time.sinceLastPress);
  Serial.print(F("ms. It was previously released "));
  Serial.print(time.sinceLastRelease);
  Serial.println(F("ms ago."));
}

void shortHold() {
  Serial.println("You held the button for 1 second.");
}

void medHold() {
  Serial.println("You held the button for 3 seconds");
}

void longHold() {
  Serial.println("You held the button for 5 seconds");
}

void onSingleClick() {
  Serial.println("Your press was a SINGLE click");
}

void onDoubleClick() {
  Serial.println("Your press was a DOUBLE click");
}

void onTripleClick() {
  Serial.println("Your press was a TRIPLE click");
}

void onReleasedAfterOne() {
  Serial.println("You held the button for at least one second, and less than two seconds.");
}

void onReleasedAfterTwo() {
  Serial.println("You held the button for at least two seconds, and less than three seconds.");
}

void onReleasedAfterThree() {
  Serial.println("You held the button for at least three seconds.");
  Serial.println("Did you notice how none of the other onReleasedAfter callbacks were called?");
}

void setup() {
  Serial.begin(115200);
  delay(250);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
/*
  // onPress callbacks are fired at the rising edge, every single time
  myBtn.setOnPressed(onPress);

  // onRelease callbacks are fired at the falling edge, every single time
  myBtn.setOnReleased(onRelease);

  // hold callbacks are fired only if the button remains pressed for the specified duration
  myBtn.setOnHold(1000, shortHold);
  myBtn.setOnHold(3000, medHold);
  myBtn.setOnHold(5000, longHold);

  // onReleasedAfter are similar to onHold, except that onHold's get called as soon as
  // their time requirements are met. onReleasedAfter's wait until the button is released,
  // and only the callback with the longest duration <= to the button press time is called
  
  // called if pressed for 1000ms <= pressedDuration < 2000ms
  myBtn.setOnReleasedAfter(1000, onReleasedAfterOne);

  // called if pressed for 2000ms <= pressedDuration < 3000ms
  myBtn.setOnReleasedAfter(2000, onReleasedAfterTwo);

  // called if pressed for pressedDuration >= 3000ms
  myBtn.setOnReleasedAfter(3000, onReleasedAfterThree);
*/

  // you can modify the multiClick timeout to allow for slower clicks
  // default is 375ms for single/double clicks
  // 600ms is set for triple clicks unless you have already set a different value
  // myBtn.setMultiClickTimeout(1000);

  // the singleClickCallback is only called if no second or third presses occurs before the MultiClickTimeout
  // note that this means single button presses will only ever fire once per MultiClickTimeout interval (unless no double or triple click handlers are set, in which case it behaves like onPress)
  myBtn.setOnSingleClick(onSingleClick);

  // if a second click occurs before the MultiClickTimeout, and no triple click handler is set,
  // the doubleClickCallback will be called and the singleClickCallback will be ignored
  // if a tripleClickCallback is set, the second click will be deferred until the end of the timeout to allow for the possibility of a third click

  myBtn.setOnDoubleClick(onDoubleClick);

  // if a triple click occurs within the timeout, the callback is fired immediately
  // and the click count resets
  // if no triple click occurs in the timeout, but two clicks are registered,
  // the double click callback will fire at the end of the timeout
  // if no double click occurs in the timeout, but a single click is registered,
  // the single click callback will fire at the end of the timeout
  myBtn.setOnTripleClick(onTripleClick);

  // Caution:
  // Use onRelease and onPress callbacks sparingly.
  // It can quickly become overwhelming if you are using too many kinds of callbacks.
  // If you need to detect double clicks, use onSingleClick and onDoubleClick without onPress and onRelease.

  // If you don't need to detect double or triple clicks,
  // consider using void onPress(EasyTimer time); and time.sinceLastPress to determine click frequency,
  // or void onRelease(EasyTimer time); and time.sinceLastPress to determine hold duration.

  // start normally
  myBtn.begin();
  
  // check that the button is held for at least 5 seconds on start, returning a bool
  // Warning: This is blocking! Blocks until duration is reached or button is released

  // if (myBtn.begin(5000)) {
  //   Serial.println(F("Button held for 5 seconds at boot"));
  // } else {
  //   Serial.println(F("Button NOT held for 5 seconds at boot"));
  // }

  // check that the button is held for at least 5 seconds on start, returning the duration that the button was held for
  // Warning: This is blocking! Blocks until duration is reached or button is released

  // unsigned long bootHoldElapsed = myBtn.begin(5000, true);
  // Serial.print(F("The button was held for "));
  // Serial.print(bootHoldElapsed);
  // Serial.println(F("ms on boot."));
}

void loop() {
  // must call in loop to keep checking the button state
  myBtn.update();
}
