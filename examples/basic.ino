/*
  EasierButton - Wrapper around EasyButton because I'm bad at cpp and Arduino.
  Created by Robert Reed, December 22, 2020.
  Released into the public domain.
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
// bool pullup = true;
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

void shortPress() {
  Serial.println("You held the button for 1 second.");
}

void medPress() {
  Serial.println("You held the button for 3 seconds");
}

void longPress() {
  Serial.println("You held the button for 5 seconds");
}

void setup() {
  Serial.begin(115200);
  delay(250);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  myBtn.setOnPressed(onPress);
  myBtn.setOnReleased(onRelease);
  myBtn.setOnHold(1000, shortPress);
  myBtn.setOnHold(3000, medPress);
  myBtn.setOnHold(5000, longPress);

  // start normally
  // myBtn.begin();
  
  // check that the button is held for at least 5 seconds on start, returning a bool
  // Warning: This is blocking! Blocks until duration is reached or button is released

  if (myBtn.begin(5000)) {
    Serial.println(F("Button held for 5 seconds at boot"));
  } else {
    Serial.println(F("Button NOT held for 5 seconds at boot"));
  }

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
