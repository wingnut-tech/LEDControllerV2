#include "LED.h"

#include "eeprom.h"
#include "pins.h"
#include "../LEDController.h"

LED Right;
LED Left;
LED Nose;
LED Fuse;
LED Tail;

uint8_t maxLeds;

void setupLeds() {
  Right.setup(layout.wing, layout.wingRev, layout.wingnav);
  Left.setup(layout.wing, layout.wingRev, layout.wingnav);
  Nose.setup(layout.nose, layout.noseRev);
  Fuse.setup(layout.fuse, layout.fuseRev);
  Tail.setup(layout.tail, layout.tailRev);

  if (! (Right.leds && Left.leds && Nose.leds && Fuse.leds && Tail.leds)) {
    while (true) {
      Serial.println(F("Too many LEDs, not enough memory"));
      delay(5000);
    }
  }

  maxLeds = max(layout.wing, max((layout.nose+layout.fuse), layout.tail));

  // initialize FastLED arrays
  FastLED.addLeds<NEOPIXEL, RIGHT_PIN>(Right.leds, layout.wing);
  FastLED.addLeds<NEOPIXEL, LEFT_PIN>(Left.leds, layout.wing);
  FastLED.addLeds<NEOPIXEL, NOSE_PIN>(Nose.leds, layout.nose);
  FastLED.addLeds<NEOPIXEL, FUSE_PIN>(Fuse.leds, layout.fuse);
  FastLED.addLeds<NEOPIXEL, TAIL_PIN>(Tail.leds, layout.tail);


  // LED power calculations
  #ifndef TMP_BRIGHTNESS
  #  define LED_POWER 18
  #  ifndef MAX_POWER
  #    define MAX_POWER 1800 // mA
  #  endif
  #  define NUM_LEDS (layout.wing + layout.wing + layout.nose + layout.fuse + layout.tail + layout.tail)
  #  define MAX_BRIGHTNESS min(255, (255 * (unsigned long)MAX_POWER) / (NUM_LEDS * LED_POWER))
  #else
  #  define MAX_BRIGHTNESS TMP_BRIGHTNESS
  #endif
  FastLED.setBrightness(MAX_BRIGHTNESS);
}

void printBrightness() {
  Serial.print(F("Max brightness: "));
  Serial.print(FastLED.getBrightness());
  Serial.println(F("/255"));
  Serial.println();
}

void toggleNavLights() {
  settings.navlights = !(bool)settings.navlights;
  updateNavlights();
  Serial.print(F("Navlights: ")); Serial.println(settings.navlights ? F("enabled") : F("disabled"));
  saveSettings();
}

void updateNavlights() {
  if (settings.navlights) {
    Right.stopPoint = Right.numLeds - Right.numNav;
    Left.stopPoint = Left.numLeds - Left.numNav;
  } else {
    Right.stopPoint = Right.numLeds;
    Left.stopPoint = Left.numLeds;
  }
}

void LED::setup(uint8_t num, bool rev, uint8_t nav) {
  leds = new CRGB[num];
  reversed = rev;
  numLeds = num;
  stopPoint = num;
  numNav = nav;
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
  for (uint8_t i = 0; i < numNav; i++) {
    if (reversed) {
      leds[i] = color;
    } else {
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
      leds[led] |= color;
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