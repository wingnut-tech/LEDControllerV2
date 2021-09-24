#pragma once

#define FASTLED_INTERNAL // disables the FastLED version message that looks like an error
#include <FastLED.h>

/**
 * @brief The main class for the different LED strip objects.
 * This mainly functions as a "wrapper" for the FastLED arrays, with some helpful methods built-in.
 * 
 */
class LED {
public:
  CRGB* leds;
  bool reversed;
  uint8_t numLeds;
  uint8_t stopPoint;

  /**
   * @brief Construct a new LED object
   * 
   * @param ledarray Pointer to the actual FastLED array this object uses
   * @param num Size of the ledArray
   * @param rev Weather this LED string is reversed from normal operation or not
   */
  LED(CRGB * ledarray, uint8_t num, bool rev);

  /**
   * @brief Sets LEDs in this object to the specified color.
   * Also handles reversing and "out-of-bounds" checking.
   * 
   * @param led 
   * @param color 
   */
  void set(uint8_t led, const CRGB& color);

  /**
   * @brief Sets navlight section of this object to specified color
   * 
   * @param color 
   */
  void setNav(const CRGB& color);

  /**
   * @brief Adds color to existing value
   * 
   * @param led 
   * @param color 
   */
  void add(uint8_t led, const CRGB& color);

  /**
   * @brief ORs the colors, making the LED the brighter of the two
   * 
   * @param led 
   * @param color 
   */
  void addor(uint8_t led, const CRGB& color);

  /**
   * @brief Fades the whole string down by the specified scale.
   * Used when needing to fade a trail to black.
   * 
   * @param scale Value from 0-255 to scale the existing colors by
   */
  void nscale8(uint8_t scale);

  /**
   * @brief 
   * 
   * @param other New color to interpolate towards
   * @param frac Value from 0-255 that specifies how much interpolation happens
   */
  void lerp8(const CRGB& other, uint8_t frac);
};