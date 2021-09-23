#pragma once

#define FASTLED_INTERNAL // disables the FastLED version message that looks like an error
#include <FastLED.h>

// Class: LED
// ----------------------------
//   the main class for the different LED strip objects
//   this mainly functions as a "wrapper" for the FastLED arrays, with some helpful methods built-in
//
//   leds: a pointer to the real FastLED array that we pass in
//   reversed: whether this strip is "reversed" from normal operation
//   numLeds: the physical number of LEDs in the strip (and array)
//   stopPoint: this is where the normal set() function will stop. this is mainly useful for navlights on the wings
class LED {
public:
  CRGB* leds;
  bool reversed;
  uint8_t numLeds;
  uint8_t stopPoint;

  // Function: LED (class constructor)
  // ---------------------------------
  //   runs when class object is initialized
  //
  //   *ledArray: pointer to the actual FastLED array this object uses
  //   num: size of the ledArray
  //   rev: weather this LED string is "reversed" from normal operation or not
  LED(CRGB * ledarray, uint8_t num, bool rev);


  // Function: set
  // -------------
  //   sets LEDs in this object to the specified color
  //   also handles reversing and "out-of-bounds" checking
  //
  //   led: which LED in the array to modify
  //   color: a CRGB color to set the LED to
  void set(uint8_t led, const CRGB& color);

  // Function: setNav
  // ----------------
  //   sets navlight section of this object to specified color
  //
  //   color: a CRGB color to set the navlights to
  void setNav(const CRGB& color);

  // Function: add
  // -------------
  //   adds color to existing value
  //
  //   led: which LED in the array to modify
  //   color: a CRGB color to set the LED to
  void add(uint8_t led, const CRGB& color);

  // Function: addor
  // ---------------
  //   "or"s the colors, making the LED the brighter of the two
  //
  //   led: which LED in the array to modify
  //   color: a CRGB color to set the LED to
  void addor(uint8_t led, const CRGB& color);

  // Function: nscale8
  // -----------------
  //   fades the whole string down by the specified scale
  //   used when needing to fade a "trail" to black
  //
  //   scale: value from 0-255 to scale the existing colors by
  void nscale8(uint8_t scale);
  // Function: lerp8
  // ---------------
  //   interpolates between existing LED color and specified color
  //
  //   other: new color to interpolate towards
  //   frac: value from 0-255 that specifies how much interpolation happens
  void lerp8(const CRGB& other, uint8_t frac);
};