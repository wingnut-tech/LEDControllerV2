#include "button.h"

#include <Arduino.h>


Button::Button(uint8_t p) {
  pin = p;
}

int Button::getPress() {
  int press = NONE;

  if (shortPress) {
    press = SHORT;
  }
  if (longPress) {
    press = LONG;
  }

  shortPress = false;
  longPress = false;
  
  return press;
}

void Button::loop() {
  int currentState = digitalRead(pin);
  unsigned long currentTime = millis();

  if (prevState == HIGH && currentState == LOW) { // pressed
    isPressed = true;
    pressTime = currentTime;
  }
  else if (prevState == LOW && currentState == HIGH) { // released
    isPressed = false;
    long duration = currentTime - pressTime;
    if (duration > 20 && duration < SHORTPRESSTIME) {
      shortPress = true;
    }
  }

  if (isPressed) {
    long duration = currentTime - pressTime;
    if (duration > LONGPRESSTIME) {
      longPress = true;
      pressTime = currentTime;
    }
  }

  prevState = currentState;
}