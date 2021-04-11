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
#define ABS_RANGE 15
#define SWITCH_TO_FAST_RANGE 4/5
#define FAST_FACTOR 0.5
#define SLOW_FACTOR 1.5

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

  // handle different speeds
  double delayFactor;
  if ((x != 0 && abs(x) >= ABS_RANGE * SWITCH_TO_FAST_RANGE)
    || (y != 0 && abs(y) >= ABS_RANGE * SWITCH_TO_FAST_RANGE))
    delayFactor = FAST_FACTOR;
  else
    delayFactor = SLOW_FACTOR;

  #ifndef DIGISPARK
    Serial.println(
      "x=" + String(x)
      + ", y=" + String(y)
      + ", delayFactor=" + String(delayFactor)
      + ", buttonState=" + (buttonState ? "HIGH" : "LOW")
      + ", buttonPressTime=" + String(buttonPressTime)
      + ", scrollMode=" + (scrollMode ? "true" : "false")
    );
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
      if (millis() - buttonPressTime < 500L) {
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
    if (scrollMode) {
      if (millis() - lastScrollTime > 250L * delayFactor) {
        lastScrollTime = millis();
        int steps;
        if (y > 0)
          steps = 5;
        else if (y < 0)
          steps = -5;
        #ifdef DIGISPARK
          DigiMouse.move(0, 0, steps);
          DigiMouse.update();
        #else
          Mouse.move(0, 0, steps);
        #endif
      }
    } else {
      if (millis() - lastMoveTime > 25L * delayFactor) {
        lastMoveTime = millis();
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