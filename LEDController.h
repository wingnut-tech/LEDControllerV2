#pragma once

#include "layout.h"

// #define TESTMODE 0 // uncomment to override RC signal and force a specific show number for testing
// #define TMP_BRIGHTNESS 55 // uncomment to override brightness for testing

// LED power calculations
// there's a difference between stock radian LEDs and the ones we're selling in kits
#ifndef TMP_BRIGHTNESS
#  ifndef LED_POWER
#    define MAX_BRIGHTNESS 255
#  else
#    define MAX_POWER 1500 // mA
#    define NUM_LEDS (WING_LEDS + WING_LEDS + NOSE_LEDS + FUSE_LEDS + TAIL_LEDS + TAIL_LEDS)
#    define MAX_BRIGHTNESS min(255, (255 * (MAX_POWER / (NUM_LEDS * LED_POWER))))
#  endif
#else
#  define MAX_BRIGHTNESS TMP_BRIGHTNESS
#endif

// this is double the actual alt limit.
// wings/fuse fill up to half this value, then "overflow" a red/orange color.
#define MAX_ALTIMETER 800

// define the pins that the buttons are connected to

#define PROGRAM_CYCLE_BTN 6
#define PROGRAM_ENABLE_BTN 7

// define the pins that the LED strings are connected to

#define TAIL_PIN 8
#define FUSE_PIN 9
#define NOSE_PIN 10
#define LEFT_PIN 11
#define RIGHT_PIN 12

// define the pins that are used for RC inputs

#define RC_PIN1 5   // Pin 5 Connected to Receiver;
#define RC_PIN2 4   // Pin 4 Connected to Receiver for optional second channel;

#define NUM_SHOWS_WITH_ALTITUDE 20 // total number of shows. 1+the last caseshow number

#define METRIC_CONVERSION 3.3; // 3.3 to convert meters to feet. 1 for meters.