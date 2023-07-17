#include "visualize.h"

#ifdef VISUALIZER

#define FASTLED_INTERNAL
#include <FastLED.h>

#include "shows.h"
#include "LED.h"
#include "mode.h"
#include "eeprom.h"


void visualize() {
  visualizeMode = true;
  currentShow = 1;

  Right.numLeds = 10;
  Left.numLeds = 10;
  Nose.numLeds = 4;
  Fuse.numLeds = 8;
  Tail.numLeds = 5;

  Right.stopPoint = 10;
  Left.stopPoint = 10;
  Nose.stopPoint = 4;
  Fuse.stopPoint = 8;
  Tail.stopPoint = 5;

  Right.reversed = false;
  Left.reversed = false;
  Nose.reversed = false;
  Fuse.reversed = false;
  Tail.reversed = false;
  layout.nosefusejoined = true;

  writeNumLeds();

  while (1) {
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input >= 0 && input < numShows) {
        currentShow = input;
        resetShow();
        continue;
      }
      else if (input == 'V') {
        writeNumLeds();
      }
      else if (input == 's' || input == 'n') {
        if (input == 's') {
          Right.numNav = 0;
          Left.numNav = 0;
          stepShow();
          Serial.write(interval);
        } else {
          Right.numNav = 3;
          Left.numNav = 3;
          navLights();
          Serial.write(30);
        }

        writeStrip(Right.leds, Right.numLeds);
        writeStrip(Left.leds, Left.numLeds);
        writeStrip(Nose.leds, Nose.numLeds);
        writeStrip(Fuse.leds, Fuse.numLeds);
        writeStrip(Tail.leds, Tail.numLeds);
      }
      else if (input == 'x') {
        break;
      }
    }
    delay(1);
  }
  visualizeMode = false;
}

void writeNumLeds() {
  Serial.write(Right.numLeds);
  Serial.write(Left.numLeds);
  Serial.write(Nose.numLeds);
  Serial.write(Fuse.numLeds);
  Serial.write(Tail.numLeds);
}

void writeStrip(CRGB* leds, uint8_t numLeds) {
  for (int i = 0; i < numLeds; i++) {
    Serial.write(leds[i].r);
    Serial.write(leds[i].g);
    Serial.write(leds[i].b);
  }
}

#endif