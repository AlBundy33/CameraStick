#ifndef ARDUINO_AVR_LEONARDO
#define DIGISPARK
#endif

#ifdef DIGISPARK
#include <DigiMouse.h>
#else
#include <Mouse.h>
#endif

// define the PINS
#define BUTTON PIN0
#define AXIS_X A1
#define AXIS_Y A0

// define input behavior
#define DEADZONE 20
#define MOVE_DELAY 10
#define SCROLL_DELAY 250
#define ABS_RANGE 15

// store states
unsigned long buttonPressTime = 0L;
unsigned long lastScrollTime = 0L;
unsigned long lastMoveTime = 0L;
bool scrollMode = false;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
#ifdef DIGISPARK
  DigiMouse.begin();
#else
  Mouse.begin();
  Serial.begin(9600);
#endif
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

  #ifndef DIGISPARK
    Serial.print("x=");
    Serial.print(x);
    Serial.print(", y=");
    Serial.print(y);
    Serial.print(", button=");
    Serial.print(buttonState ? "HIGH" : "LOW");
    Serial.print(", buttonPressTime=");
    Serial.print(buttonPressTime);
    Serial.print(", scrollMode=");
    Serial.print(scrollMode ? "true" : "false");
    Serial.println();
  #endif

  // check button
  if (buttonState == LOW) {
    // Button pressed
    if (buttonPressTime == 0L) {
      buttonPressTime = millis();
    }
  } else {
    if (buttonPressTime > 0L) {
      // Button released
      if (millis() - buttonPressTime < (long)500) {
        // short press - change camera
        #ifdef DIGISPARK
          DigiMouse.setButtons(MOUSEBTN_LEFT_MASK);
          DigiMouse.delay(20);
          DigiMouse.setButtons(0);
        #else
          Mouse.click(MOUSE_LEFT);
        #endif
      } else {
        // long press - switch mode
        scrollMode = !scrollMode;
      }
      buttonPressTime = 0L;
    }
  }

  // move or scroll mouse
  if (x != 0 || y != 0) {
    unsigned long ms = millis();
    if (scrollMode) {
      if (ms - lastScrollTime > (long)SCROLL_DELAY) {
        lastScrollTime = ms;
        #ifdef DIGISPARK
          DigiMouse.move(0, 0, y);
          DigiMouse.update();
        #else
          Mouse.move(0, 0, y);
        #endif
      }
    } else {
      if (ms - lastMoveTime > (long)MOVE_DELAY) {
        lastMoveTime = ms;
        #ifdef DIGISPARK
          DigiMouse.move(x, y, 0);
          DigiMouse.update();
        #else
          Mouse.move(x, y, 0);
        #endif
      }
    }
  }
}