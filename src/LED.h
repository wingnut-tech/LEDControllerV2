#pragma once

#define FASTLED_INTERNAL // disables the FastLED version message that looks like an error
#include <FastLED.h>

class LED {
public:
  CRGB* leds;
  bool reversed;
  uint8_t numLeds;
  uint8_t stopPoint;
  uint8_t numNav;

  void setup(uint8_t num, bool rev, uint8_t nav=0);

  void set(uint8_t led, const CRGB& color);

  void setNav(const CRGB& color);

  void add(uint8_t led, const CRGB& color);

  void addor(uint8_t led, const CRGB& color);

  void nscale8(uint8_t scale);

  void lerp8(const CRGB& other, uint8_t frac);
};


void setupLeds();

void printBrightness();

void toggleNavLights();

void updateNavlights();

extern LED Right;
extern LED Left;
extern LED Nose;
extern LED Fuse;
extern LED Tail;

extern uint8_t maxLeds;