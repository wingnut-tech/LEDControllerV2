#pragma once

#define FASTLED_INTERNAL
#include <FastLED.h>

// Function: stepShow
// ------------------
//   this is the main "show rendering" update function
//   this plays the next "frame" of the current show
void stepShow();

// Function: blank
// ---------------
//   Turn off all LEDs
void blank();

// Function: setColor(CRGB)
// ------------------
//   sets all LEDs to a solid color
//
//   color: a CRGB color to set the LEDs to
void setColor(const CRGB& color);

// Function: setColor(CRGBPalette16)
// ------------------
//   spreads a palette across all LEDs
//
//   palette: pre-defined CRGBPalette16 object that will be used
void setColor (const CRGBPalette16& palette);

// Function: LetterToColor
// -----------------------
//   convert the letters in the static patterns to color values
//
//   letter: a single char string that will become an actual CRGB value
//
//   returns: a new CRGB color
CRGB LetterToColor (char letter);

// Function: setPattern
// --------------------
//   sets wings to a static pattern
//
//   pattern: pre-defined static pattern array
void setPattern (char pattern[]);

// Function: setInitPattern
// ------------------------
//   set all LEDs to the static init pattern
void setInitPattern ();

// Function: setNoseFuse
// ---------------------
//   sets leds along nose and fuse as if they were the same strip
//   range is 0 - ((NOSE_LEDS+FUSE_LEDS)-1)
//
//   led: which LED to modify
//   color: a CRGB color to set the LED to
//   (optional) addor: set true to "or" the new led color with the old value. if not set, defaults to false
// void setNoseFuse(uint8_t led, const CRGB& color) { setNoseFuse(led, color, false); } // overload for simple setting of leds
void setNoseFuse(uint8_t led, const CRGB& color, bool addor=false);
// Function: setBothWings
// ----------------------
//   sets leds along both wings as if they were the same strip
//   range is 0 - ((stopPoint*2)-1). left.stopPoint = 0, right.stopPoint = max
//
//   led: which LED to modify
//   color: a CRGB color to set the LED to
//   (optional) addor: set true to "or" the new led color with the old value. if not set, defaults to false
// void setBothWings(uint8_t led, const CRGB& color) { setBothWings(led, color, false); } // overload for simple setting of leds
void setBothWings(uint8_t led, const CRGB& color, bool addor=false);

// Function: animateColor
// ----------------------
//   animates a palette across all LEDs
//
//   palette: pre-defined CRGBPalette16 object that will be used
//   ledOffset: how much each led is offset in the palette compared to the previous led
//   stepSize: how fast the leds will cycle through the palette
void animateColor (const CRGBPalette16& palette, int ledOffset, int stepSize);

// Function: colorWave1
// --------------------
//   rainbow pattern
//
//   ledOffset: how much each led is offset in the rainbow compared to the previous led
//   l_interval: sets interval for this show
void colorWave1 (uint8_t ledOffset, uint8_t l_interval);

// Function: chase/cylon
// ---------------------
//   LED chase functions with fadeout of the tail
//   can do more traditional same-direction chase, or back-and-forth "cylon/knight-rider" style
//
//   color1: "main" color of the chase effect
//   color2: "background" color
//   speedWing: chase speed of the wing leds
//   speedNose: chase speed of the nose leds
//   speedFuse: chase speed of the fuse leds
//   speedTail: chase speed of the tail leds
//   (optional) cylon: if this is set, does back-and-forth effect. regular chase otherwise. overloads set this.
// void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail) { // overload to do a chase pattern
//   chase(color1, color2, speedWing, speedNose, speedFuse, speedTail, false);
// }
// // overload to do a cylon pattern
// void cylon(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail) {
//   chase(color1, color2, speedWing, speedNose, speedFuse, speedTail, true);
// }
// main chase function. can do either chase or cylon patterns
void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail, bool cylon=false);

// Function: juggle
// ----------------
//   a few "pulses" of light that bounce back and forth at different timings
//
//   numPulses: how many unique pulses per string
//   speed: how fast the pulses move back and forth
void juggle(uint8_t numPulses, uint8_t speed);

// Function: navLights
// -------------------
//   main function that animates the persistent navlights
void navLights();

// Function: strobe
// ----------------
//   various strobe patterns
//
//   style: which strobe style to use
//     1: rapid strobing all LEDS in unison. bad. might cause seizures
//     2: alternate strobing of left and right wing
//     3: alternate double-blink strobing of left and right wing
void strobe(int style);

// Function: altitude
// ------------------
//   altitude indicator show
//   wings fill up to indicate altitude, tail goes green/red as variometer
//
//   fake: set to 0 for real data, anything else for testing
//   palette: gradient palette for the visual variometer on the tail
void altitude(double fake, const CRGBPalette16& palette);

// Enum: twinkle
// ----------------------------
//   some named "states" for the twinkle functions
enum {SteadyDim, Dimming, Brightening};

// Function: doTwinkle1
// --------------------
//   helper function for the twinkle show
//
//   ledArray: pointer to the led array we're modifying
//   pixelState: pointer to the array that holds state info for the led array
//   size: size of the led array
void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size);

// Function: twinkle1
// ----------------------------
//   random twinkle effect on all LEDs
void twinkle1();

// Function: statusFlash overloads
// -------------------------------
//   various overload versions of the statusFlash function
// void statusFlash(bool status);
// void statusFlash(bool status, uint8_t numFlashes, int delay_time);
// void statusFlash(char status);

// Function: statusFlash
// ---------------------
//   flashes red/green/white for different program mode indicators
//
//   status: single char ('w', 'g', or 'r') that specifies what color to flassh all leds
//   numFlashes: how many on/off cycles to do
//   delay_time: delay after both the 'on' and 'off' states
void statusFlash(uint8_t status, uint8_t numFlashes=4, int delay_time=50);
