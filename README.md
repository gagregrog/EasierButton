# Easier Button

**Disclaimer:** I do not know how to properly write c++, so you should probably not use this library.

Wrapper around EasyButton since some of the API doesn't seem to work for me. Most likely user error, yet here we are.

## Vector Compile Errors

If you are on an AVR board such as an Arduino Micro, you must include the [`<ArduinoSTL.h>`](https://github.com/mike-matera/ArduinoSTL) library **before** including `EasierButton`, otherwise everything will crash and burn.

```
#include <Arduino.h>
#include <ArduinoSTL.h> // mike-matera/ArduinoSTL@^1.1.0
#include <EasierButton.h>

.
.
.
```
