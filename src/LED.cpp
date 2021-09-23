#include "../LEDController.h"
#include "LED.h"


LED::LED(CRGB * ledarray, uint8_t num, bool rev) {
  reversed = rev;
  numLeds = num;
  stopPoint = num;
  leds = ledarray; // Sets the internal 'leds' pointer to point to the "real" led array
}

void LED::set(uint8_t led, const CRGB& color) {
  if (led < stopPoint) {
    if (reversed) {
      leds[numLeds - led - 1] = color;
    } else {
      leds[led] = color;
    }
  }
}

void LED::setNav(const CRGB& color) {
  for (uint8_t i = 0; i < WING_NAV_LEDS; i++) {
    if (reversed) { // if reversed, start at the "beginning" of the led string
      leds[i] = color;
    } else { // not reversed, so start at the "end" of the string and work inwards.
      leds[numLeds - i - 1] = color;
    }
  }
}

void LED::add(uint8_t led, const CRGB& color) {
  if (led < stopPoint) {
    if (reversed) {
      leds[numLeds - led - 1] += color;
    } else {
      leds[led] += color;
    }
  }
}

void LED::addor(uint8_t led, const CRGB& color) {
  if (led < stopPoint) {
    if (reversed) {
      leds[numLeds - led - 1] |= color;
    } else {
      leds[led] += color;
    }
  }
}

void LED::nscale8(uint8_t scale) {
  for (uint8_t i = 0; i < stopPoint; i++) {
    if (reversed) {
      leds[numLeds - i - 1] = leds[numLeds - i - 1].nscale8(scale);
    } else {
      leds[i] = leds[i].nscale8(scale);
    }
  }
}

void LED::lerp8(const CRGB& other, uint8_t frac) {
  for (uint8_t i = 0; i < stopPoint; i++) {
    if (reversed) {
      leds[numLeds - i - 1] = leds[numLeds - i - 1].lerp8(other, frac);
    } else {
      leds[i] = leds[i].lerp8(other, frac);
    }
  }
}