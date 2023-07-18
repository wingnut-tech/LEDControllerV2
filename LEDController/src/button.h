#pragma once

#include <stdint.h>

enum {NONE, SHORT, LONG};

#define LONGPRESSTIME 3000
#define SHORTPRESSTIME 1000

class Button {
private:
  bool isPressed;
  bool shortPress;
  bool longPress;
  uint8_t prevState;
  unsigned long pressTime;
public:
  uint8_t pin;

  Button(uint8_t p);

  int getPress();

  void loop();
};