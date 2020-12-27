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
// or when canceled via a hold callback
void onRelease(EasyTimer time) {
  Serial.print(F("You released the button! It was held for "));
  Serial.print(time.sinceLastPress);
  Serial.print(F("ms. It was previously released "));
  Serial.print(time.sinceLastRelease);
  Serial.println(F("ms ago."));
}

void shortestHold() {
  Serial.println("onHold: You held the button for 1 second. Since strict = false, I didn't cancel any other callbacks.");
}

void shortHold() {
  Serial.println("You held the button for 5 seconds.");
}

void medHold() {
  Serial.println("You held the button for 10 seconds");
}

void longHold() {
  Serial.println("You held the button for 15 seconds");
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

void onReleasedAfterLoose() {
  Serial.println("You held the button for at least one half second, and less than one second. Notice that the multiClick callback got called too, since I'm not in strict mode.");
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

  // onPress callbacks are fired at the rising edge, every single time
  // only one onPress callback is permitted (overwrite if called again)
  myBtn.setOnPressed(onPress);

  // onRelease callbacks are fired at the falling edge, every single time (unless canceled)
  // only one onRelease callback is permitted (overwrite if called again)
  myBtn.setOnReleased(onRelease);

  // hold callbacks are fired only if the button remains pressed for the specified duration
  // hold callbacks do not cancel future hold callbacks, each of these will be called if their hold duration is met
  myBtn.setOnHold(5000, shortHold);
  myBtn.setOnHold(10000, medHold);
  myBtn.setOnHold(15000, longHold);

  // hold callbacks are strict by default
  // this means if they are called they cancel the subsequent onRelease callbacks
  // specifically, they will cancel onRelease and onReleaseAfter triggered on the next release
  // to override this behavior, provide a false flag to the setter
  myBtn.setOnHold(1000, shortestHold, false);

  // onReleasedAfter callbacks are similar to onHold callbacks, 
  // except that onHold's get called as soon as their time requirements are met.
  // onReleasedAfter callbacks wait until the button is released,
  // and only the callback with the longest duration <= to the button press time is called

  // since onReleasedAfter callbacks are canceled by onHold strict mode,
  // you'd be best off using a duration less than your onHold durations (if they are strict)
  // called if pressed for 1000ms <= pressedDuration < 2000ms
  myBtn.setOnReleasedAfter(1000, onReleasedAfterOne);

  // called if pressed for 2000ms <= pressedDuration < 3000ms
  myBtn.setOnReleasedAfter(2000, onReleasedAfterTwo);

  // called if pressed for pressedDuration >= 3000ms
  myBtn.setOnReleasedAfter(3000, onReleasedAfterThree);

  // like onHold callbacks, onReleasedAfter callbacks are in strict mode by default
  // this means that if called, they will cancel upcoming click callbacks
  // for example, if you do a single click followed by a strict onReleasedAfter cb,
  // the click callback will not be called when it times out
  // to override this behavior, pass false when setting the cb
  myBtn.setOnReleasedAfter(500, onReleasedAfterLoose);

  // If you're more interested in clicks than holds, you can use the multiClick setter

  // the singleClickCallback is only called if no second or third presses occurs before the MultiClickTimeout
  // note that this means single button presses will only ever fire once per MultiClickTimeout interval (unless no double or triple click handlers are set, in which case it behaves like onPress)
  // note that these callbacks will not be called if a strict onHoldAfter has been called
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

  // you can modify the multiClick timeout to allow for slower clicks
  // default is 375ms for single/double clicks
  // 600ms is set for triple clicks unless you have already set a different value
  myBtn.setMultiClickTimeout(1000);

  // To distinguish clicks from holds, clicks must be held for less than 500ms
  // you can override this value as needed
  myBtn.defineMaxClickDuration(400);


  // Caution:
  // Use onRelease and onPress callbacks sparingly.
  // It can quickly become overwhelming if you are using too many kinds of callbacks.

  // If you don't need to detect double or triple clicks,
  // consider using void onPress(EasyTimer time); and time.sinceLastPress to determine click frequency,
  // or void onRelease(EasyTimer time); and time.sinceLastPress to determine hold duration.

  // Setting up the button

  // You can start normally
  myBtn.begin();
  
  // or you can check that the button is held for a specified duration on start,
  // returning a bool
  // Warning: This is blocking! Blocks until duration is reached or button is released

  // if (myBtn.begin(5000)) {
  //   Serial.println(F("Button held for 5 seconds at boot"));
  // } else {
  //   Serial.println(F("Button NOT held for 5 seconds at boot"));
  // }

  // or you can check that the button is held for a duration,
  // returning the duration that the button was held for
  // Warning: This is blocking! Blocks until duration is reached or button is released

  // unsigned long bootHoldElapsed = myBtn.begin(5000, true);
  // Serial.print(F("The button was held for "));
  // Serial.print(bootHoldElapsed);
  // Serial.println(F("ms on boot."));

  // or you can use the getHoldDuration helper to build more complex logic
  // pass boot=true to add hold duration to the heldAtBoot variable
  // if (myBtn.getHoldDuration(1000, true) >= 1000) {
    // Serial.println("Button held for 1000ms at boot");
    // if (myBtn.getHoldDuration(1000, true) {
      // Serial.println("Button held for 2000ms at boot");
    // }
  // }

  // but if you only need to know if a hold was at least a certain duration, use heldFor
  // you can also pass boot = true to add the duration to the heldAtBoot variable
  // if (myBtn.heldFor(1000)) {
    // Serial.println("Button held for 1000ms at boot");
    // if (myBtn.heldFor(1000) {
      // Serial.println("Button held for 2000ms at boot");
    // }
  // }

  // another useful function is waitForClick
  // this is blocking, but can be a useful escape hatch
  // bool selfDestruct = false;
  // if (myBtn.heldFor(2000)) {
  //   Serial.println("Self destructing in 5 seconds unless you click to cancel");

  //   if (myBtn.waitForClick(5000)) {
  //     Serial.println("Self destruct aborted!")
  //   } else {
  //     Serial.println("__SELF_DESTRUCT__");
  //   }
  // }

  if (myBtn.heldAtBoot) {
    Serial.print("Button was held for ");
    Serial.print(muBtn.heldAtBoot);
    Serial.println("ms at boot.");
  } else if (myBtn.pressedAtBoot) {
    Serial.println("Button was pressed at boot!");
  }
}

void loop() {
  // must call in loop to keep checking the button state
  myBtn.update();
}
