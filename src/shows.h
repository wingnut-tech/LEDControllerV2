#pragma once

#define FASTLED_INTERNAL
#include <FastLED.h>

#define NUM_SHOWS_WITH_ALTITUDE 20 // total number of shows. 1+the last caseshow number

/**
 * @brief This is the main show rendering update function. It plays the next "frame" of the current show.
 * 
 */
void stepShow();

// Function: blank
// ---------------
/**
 * @brief Turn off all LEDs
 * 
 */
void blank();

/**
 * @brief Sets all LEDs to a solid color
 * 
 * @param color 
 */
void setColor(const CRGB& color);

/**
 * @brief Spreads a palette across all LEDs
 * 
 * @param palette 
 */
void setColor (const CRGBPalette16& palette);

/**
 * @brief Convert the letters in static patterns to color values
 * 
 * @param letter 
 * @return CRGB 
 */
CRGB LetterToColor (char letter);

/**
 * @brief Sets wings to a static pattern
 * 
 * @param pattern 
 */
void setPattern (char pattern[]);

/**
 * @brief Set all LEDs to the static init pattern
 * 
 */
void setInitPattern ();

/**
 * @brief Sets LEDs along nose and fuse as if they were the same strip. Range is 0 - ((NOSE_LEDS+FUSE_LEDS)-1).
 * 
 * @param led 
 * @param color 
 * @param addor (optional) Adds new color to current color
 */
void setNoseFuse(uint8_t led, const CRGB& color, bool addor=false);

/**
 * @brief Sets leds along both wings as if they were the same strip. Range is 0 - ((stopPoint*2)-1).
 * 
 * @param led 
 * @param color 
 * @param addor (optional) Adds new color to current color
 */
void setBothWings(uint8_t led, const CRGB& color, bool addor=false);

/**
 * @brief Animates a palette across all LEDs
 * 
 * @param palette 
 * @param ledOffset 
 * @param stepSize 
 */
void animateColor (const CRGBPalette16& palette, int ledOffset, int stepSize);

/**
 * @brief Rainbow pattern
 * 
 * @param ledOffset 
 * @param l_interval 
 */
void colorWave1 (uint8_t ledOffset, uint8_t l_interval);

/**
 * @brief LED chase functions with fadeout of the tail. Can do more traditional same-direction chase, or back-and-forth "cylon/knight-rider" style.
 * 
 * @param color1 
 * @param color2 
 * @param speedWing 
 * @param speedNose 
 * @param speedFuse 
 * @param speedTail 
 * @param cylon (optional) If this is set, does back-and-forth effect. Regular chase otherwise.
 */
void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail, bool cylon=false);

/**
 * @brief A few "pulses" of light that bounce back and forth at different timings
 * 
 * @param numPulses 
 * @param speed 
 */
void juggle(uint8_t numPulses, uint8_t speed);

/**
 * @brief Main function that animates the persistent navlights
 * 
 */
void navLights();

/**
 * @brief Various strobe patters
 * 
 * @param style
 * 1: Rapid strobing all LEDS in unison. Bad. Might cause seizures.
 * 2: Alternate strobing of left and right wing.
 * 3: Alternate double-blink strobing of left and right wing.
 * 
 * test
 * 
 * 
 */
void strobe(int style);

/**
 * @brief Altitude indicator show. Wings fill up to indicate altitude, tail goes green/red as variometer.
 * 
 * @param palette 
 * @param fake (optional) Defaults to 0 for real data, set to anything else for testing
 */
void altitude(const CRGBPalette16& palette, double fake=0);

/**
 * @brief Some named states for the twinkle functions
 * 
 */
enum {SteadyDim, Dimming, Brightening};

/**
 * @brief Helper function for the twinkle show
 * 
 * @param ledArray Pointer to the led array we're modifying
 * @param pixelState Pointer to the array that holds state info for the led array
 * @param size Size of the led array
 */
void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size);

/**
 * @brief Random twinkle effect on all LEDs
 * 
 */
void twinkle1();

/**
 * @brief Flashes red/green/white for different program mode indicators
 * 
 * @param status Single char ('w', 'g', or 'r') that specifies what color to flash all leds
 * @param numFlashes How many on/off cycles to do
 * @param delay_time Delay after both the 'on' and 'off' states
 */
void statusFlash(uint8_t status, uint8_t numFlashes=4, int delay_time=50);
