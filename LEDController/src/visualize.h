#pragma once

#include "../LEDController.h"

#ifdef VISUALIZER

#define FASTLED_INTERNAL
#include <FastLED.h>
#include <stdint.h>

void visualize();

void writeNumLeds();

void writeStrip(CRGB* leds, uint8_t numLeds);

#endif