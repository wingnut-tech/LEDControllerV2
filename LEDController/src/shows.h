#pragma once

#define FASTLED_INTERNAL
#include <FastLED.h>

#define NUM_SHOWS_WITH_ALTITUDE 20 // total number of shows. 1+the last caseshow number

extern uint8_t numShows; // numShows becomes 1 less if no BMP280 module is installed
extern uint8_t numActiveShows; // how many actual active shows
extern uint8_t activeShowNumbers[]; // our array of currently active show switchcase numbers
extern uint8_t currentShow;
extern int interval;

void stepShow();

uint8_t beat(uint16_t bpm);

void resetShow();

void setColor(const CRGB& color);

void setColor(const CRGBPalette16& palette);

void setNoseFuse(uint8_t led, const CRGB& color, bool addor=false);

void setBothWings(uint8_t led, const CRGB& color, bool addor=false);

void animateColor(const CRGBPalette16& palette, int ledOffset, int stepSize);

void colorWave1(uint8_t ledOffset);

void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail, bool cylon=false);

void juggle(uint8_t numPulses, uint8_t speed);

void navLights();

void strobe(int style);

void altitude(const CRGBPalette16& palette, double fake=0);

void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size, bool reset);

void twinkle1(bool reset);

void statusFlash(uint8_t status, uint8_t numFlashes=4, int delay_time=50);

void noSignal();