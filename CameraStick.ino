#include <DigiMouse.h>

#define BUTTON PB0
#define AXIS_X A1
#define AXIS_Y A0
// to use A0 you have to enable the pin as input pin
// https://wolles-elektronikkiste.de/digispark-die-bequeme-attiny85-alternative
// https://digistump.com/wiki/digispark/tutorials/programming
// /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C /Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -p attiny85 -c avrisp -P /dev/cu.usbmodem14201 -b 19200 -n   
// /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C /Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -p attiny85 -c avrisp -P /dev/cu.usbmodem14201 -b 19200 -U hfuse:w:0x5F:m

#define DEADZONE 20
#define DELAY 10
#define ABS_RANGE 15

int buttonPressTime = -1;
bool scrollMode = false;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  DigiMouse.begin();
}

void loop() {
  // get values
  int x = map(analogRead(AXIS_X), 0, 1023, -512, 512);
  int y = map(analogRead(AXIS_Y), 0, 1023, -512, 512);
  int buttonState = digitalRead(BUTTON);

  // handle deadzone
  if (abs(x) <= DEADZONE)
    x = 0;
  if (abs(y) <= DEADZONE)
    y = 0;

  // scale range
  x = map(x, -512, 512, -1 * ABS_RANGE, ABS_RANGE);
  y = map(y, -512, 512, -1 * ABS_RANGE, ABS_RANGE);

  // check button
  if (buttonState == LOW) {
    // Button pressed
    if (buttonPressTime < 0) {
      buttonPressTime = millis();
    }
  } else {
    if (buttonPressTime > 0) {
      // Button released
      if ((millis() - buttonPressTime) < 500) {
        // short press - change camera
        DigiMouse.setButtons(MOUSEBTN_LEFT_MASK);
        DigiMouse.delay(20);
        DigiMouse.setButtons(0);
      } else {
        // long press - switch mode
        scrollMode = !scrollMode;
      }
      buttonPressTime = -1;
    }
  }

  // move or scroll mouse
  if (x != 0 || y != 0) {
    if (scrollMode)
      DigiMouse.move(0, 0, y);
    else
      DigiMouse.move(x, y, 0);
    DigiMouse.delay(DELAY);
  }
}
