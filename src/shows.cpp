#include "shows.h"

#include "../LEDController.h"

#include "LED.h"
#include "patterns.h"
#include "eeprom.h"
#include "pins.h"
#include "bmx280.h"
#include "mode.h"

#define FASTLED_INTERNAL
#include <FastLED.h>


uint8_t numShows = NUM_SHOWS_WITH_ALTITUDE; // numShows becomes 1 less if no BMP280 module is installed
uint8_t numActiveShows = numShows; // how many actual active shows
uint8_t activeShowNumbers[NUM_SHOWS_WITH_ALTITUDE]; // our array of currently active show switchcase numbers
uint8_t currentShow = 0;
int interval = 20;

static uint8_t prevShow;
static int currentStep = 0; // this is just a global general-purpose counter variable that any show can use for whatever it needs
static bool reset = false;
static uint32_t frame = 0;

void stepShow() {
  if (!haveSignal && !programMode && !visualizeMode) {
    interval = 20;
    noSignal();
    return;
  }
  digitalWrite(STATUS_LED, LOW);

  #define caseshow(x,y,i) case x: y; interval=i; break; // macro for switchcases with a built-in break
  
  if (currentShow != prevShow) { // did we just switch to a new show?
    resetShow();
  }

  int switchShow; // actual show to select in the switchcase
  if (programMode || visualizeMode) { // if we're in program mode, select from all shows
    switchShow = currentShow;
  } else { // if not in program mode, only select from enabled shows
    switchShow = activeShowNumbers[currentShow];
  }

  switch (switchShow) { // activeShowNumbers[] will look like {1, 4, 5, 9}, so this maps to actual show numbers
    // make sure to update NUM_SHOWS_WITH_ALTITUDE in shows.h
    // if adding or removing shows
    
    // format:
    //  (shownum,   function(args),    interval)
    caseshow(0,  setColor(CRGB::Black), 50); // all off except for NAV lights, if enabled
    caseshow(1,  colorWave1(10), 10); // regular rainbow
    caseshow(2,  colorWave1(0), 10); // zero led offset, so the whole plane is a solid color rainbow
    caseshow(3,  setColor(CRGB::Red), 50); // whole plane solid col, 50or
    caseshow(4,  setColor(CRGB::DarkOrange), 50);
    caseshow(5,  setColor(CRGB::Yellow), 50);
    caseshow(6,  setColor(CRGB::Green), 50);
    caseshow(7,  setColor(CRGB::Blue), 50);
    caseshow(8,  setColor(CRGB::Indigo), 50);
    caseshow(9,  setColor(CRGB::DarkCyan), 50);
    caseshow(10, setColor(CRGB::White), 50);
    caseshow(11, twinkle1(reset), 10); // twinkle effect
    caseshow(12, strobe(3), 50); // Realistic double strobe alternating between wings
    caseshow(13, strobe(2), 50); // Realistic landing-light style alternating between wings
    caseshow(14, strobe(1), 50); // unrealistic rapid strobe of all non-nav leds, good locator/identifier. also might cause seizures
    caseshow(15, chase(CRGB::White, CRGB::Black, 50, 80, 35, 80), 10); // chase effect, with a white streak on a black background
    caseshow(16, chase(CRGB::Red, CRGB::Black, 30, 50, 30, 50, true), 10); // Night Rider/Cylon style red beam scanning back and forth
    caseshow(17, juggle(4, 8), 10); // multiple unique pulses of light bouncing back and forth, all with different colors
    caseshow(18, animateColor(USA, 4, 1), 20); // sweeps a palette across the whole plane

    // altitude needs to be the last show so we can disable it if no BMP280 module is installed
    caseshow(19, altitude(variometer), 100); // second parameter can be to another number for "fake" altitude
  }
  reset = false;
  prevShow = currentShow;
  frame++;
}

uint8_t beat(uint16_t bpm) {
  return (frame*interval * (bpm << 8) * 280) >> 24;
}

void resetShow() {
  reset = true;
  currentStep = 0; // reset the global general-purpose counter
  setColor(CRGB::Black);
  if (visualizeMode) {
    frame = 0;
  } else {
    Serial.print(F("Current Show: "));
    Serial.println(currentShow);
    if (programMode) { // if we're in program mode and just switched, indicate show status
      statusFlash(settings.enabledShows[currentShow]); //flash all LEDs red/green to indicate current show status
    }
  }
}

void setColor(const CRGB& color) {
  fill_solid(Right.leds, Right.stopPoint, color);
  fill_solid(Left.leds, Left.stopPoint, color);
  fill_solid(Nose.leds, Nose.stopPoint, color);
  fill_solid(Fuse.leds, Fuse.stopPoint, color);
  fill_solid(Tail.leds, Tail.stopPoint, color);
  FastLED.show();
}

void setColor(const CRGBPalette16& palette) {
  for (int i = 0; i < maxLeds; i++) {
    // range of 0-240 is used in the map() function due to how the FastLED ColorFromPalette() function works.
    // 240 is actually the correct wrap-around point
    Right.set(i, ColorFromPalette(palette, map(i, 0, Right.stopPoint, 0, 240)));
    Left.set(i, ColorFromPalette(palette, map(i, 0, Left.stopPoint, 0, 240)));
    Tail.set(i, ColorFromPalette(palette, map(i, 0, Tail.stopPoint, 0, 240)));
    if (layout.nosefusejoined) {
      setNoseFuse(i, ColorFromPalette(palette, map(i, 0, Nose.stopPoint+Fuse.stopPoint, 0, 240)));
    } else {
      Nose.set(i, ColorFromPalette(palette, map(i, 0, Nose.stopPoint, 0, 240)));
      Fuse.set(i, ColorFromPalette(palette, map(i, 0, Fuse.stopPoint, 0, 240)));
    }
  }
  FastLED.show();
}

void setNoseFuse(uint8_t led, const CRGB& color, bool addor) {
  if (led < Nose.stopPoint) { // less than nose stopPoint, set the nose
    if (addor) {
      Nose.addor(led, color);
    } else {
      Nose.set(led, color);
    }
  } else { // greater than nose stopPoint, set fuse (minus nose stopPoint)
    if (addor) {
      Fuse.addor(led-Nose.stopPoint, color);
    } else {
      Fuse.set(led-Nose.stopPoint, color);
    }
  }
}

void setBothWings(uint8_t led, const CRGB& color, bool addor) {
  if (led < Left.stopPoint) { // less than left size, set left wing, but reversed (start at outside)
    if (addor) {
      Left.addor(Left.stopPoint - led - 1, color);
    } else {
      Left.set(Left.stopPoint - led - 1, color);
    }
  } else { // greater than left size, set right wing
    if (addor) {
      Right.addor(led - Left.stopPoint, color);
    } else {
      Right.set(led - Left.stopPoint, color);
    }
  }
}

void animateColor(const CRGBPalette16& palette, int ledOffset, int stepSize) {
  if (currentStep > 255) {currentStep -= 256;}
  for (uint8_t i = 0; i < maxLeds; i++) {
    // scale to 240 again because that's the correct wrap point for ColorFromPalette()
    CRGB color = ColorFromPalette(palette, scale8(triwave8((i * ledOffset) + currentStep), 240));
    Right.set(i, color);
    Left.set(i, color);
    Tail.set(i, color);
    if (layout.nosefusejoined) {
      setNoseFuse(i, color);
    } else {
      Nose.set(i, color);
      Fuse.set(i, color);
    }
  }

  currentStep += stepSize;
  FastLED.show();
}

void colorWave1(uint8_t ledOffset) {
  if (currentStep > 255) {currentStep = 0;}
  for (uint8_t i = 0; i < Left.stopPoint + Right.stopPoint; i++) {
    setBothWings(i, CHSV(currentStep + (ledOffset * i), 255, 255));
  }
  for (uint8_t i = 0; i < maxLeds; i++) {
    // the CHSV() function uses uint8_t, so wrap-around is already taken care of
    CRGB color = CHSV(currentStep + (ledOffset * i), 255, 255);
    Tail.set(i, color);
    if (layout.nosefusejoined) {
      setNoseFuse(i, color);
    } else {
      Nose.set(i, color);
      Fuse.set(i, color);
    }
  }
  currentStep++;
  FastLED.show();
}

void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail, bool cylon) {
  // fade out the whole string to get a nice fading trail
  if (color2 == (CRGB)CRGB::Black) { // if our second color is black, do nscale8, because lerp never gets there
    Right.nscale8(192);
    Left.nscale8(192);
    Fuse.nscale8(192);
    Nose.nscale8(192);
    Tail.nscale8(192);
  } else { // otherwise, just lerp between the colors 
    Right.lerp8(color2, 20);
    Left.lerp8(color2, 20);
    Fuse.lerp8(color2, 20);
    Nose.lerp8(color2, 20);
    Tail.lerp8(color2, 20);
  }

  if (cylon == true) {
    // cylon uses both wings, and triwave to get a nice back-and-forth
    setBothWings(scale8(triwave8(beat(speedWing)), (Right.stopPoint+Left.stopPoint)-1), color1);
    Tail.set(scale8(triwave8(beat(speedTail)), Tail.stopPoint-1), color1);
    if (layout.nosefusejoined) {
      setNoseFuse(scale8(triwave8(beat(speedFuse)), (Nose.stopPoint+Fuse.stopPoint)-1), color1);
    } else {
      Nose.set(scale8(triwave8(beat(speedNose)), Nose.stopPoint-1), color1);
      Fuse.set(scale8(triwave8(beat(speedFuse)), Fuse.stopPoint-1), color1);
    }
  } else {
    // chase just goes out in the same direction
    Right.set(scale8(beat(speedWing), Right.stopPoint-1), color1);
    Left.set(scale8(beat(speedWing), Left.stopPoint-1), color1);
    Tail.set(scale8(beat(speedTail), Tail.stopPoint-1), color1);
    if (layout.nosefusejoined) {
      setNoseFuse(scale8(beat(speedFuse), (Nose.stopPoint+Fuse.stopPoint)-1), color1);
    } else {
      Nose.set(scale8(beat(speedNose), Nose.stopPoint-1), color1);
      Fuse.set(scale8(beat(speedFuse), Fuse.stopPoint-1), color1);
    }
  }

  FastLED.show();
}

void juggle(uint8_t numPulses, uint8_t speed) {
  uint8_t spread = 256 / numPulses;

  // fade out the whole string to get a nice fading trail
  Right.nscale8(192);
  Left.nscale8(192);
  Fuse.nscale8(192);
  Nose.nscale8(192);
  Tail.nscale8(192);

  for (uint8_t i = 0; i < numPulses; i++) {
    // use addor on everything, so colors add when overlapping
    setBothWings(scale8(sin8(beat(i+speed)), (Right.stopPoint+Left.stopPoint)-1), CHSV(i*spread + beat(1), 200, 255), true);
    Tail.addor(scale8(sin8(beat(i+speed)), Tail.stopPoint-1), CHSV(i*spread + beat(1), 200, 255));
    if (layout.nosefusejoined) {
      setNoseFuse(scale8(sin8(beat(i+speed)), (Nose.stopPoint+Fuse.stopPoint)-1), CHSV(i*spread + beat(1), 200, 255), true);
    } else {
      Nose.addor(scale8(sin8(beat(i+speed)), Nose.stopPoint-1), CHSV(i*spread + beat(1), 200, 255));
      Fuse.addor(scale8(sin8(beat(i+speed)), Fuse.stopPoint-1), CHSV(i*spread + beat(1), 200, 255));
    }
  }

  FastLED.show();
}

void navLights() {
  static uint8_t navStrobeState = 0;

  if (!visualizeMode && (!haveSignal || !settings.navlights)) {
    return;
  }

  switch(navStrobeState) {
    case 50:
    case 51:
    case 54:
    case 55:
      Left.setNav(CRGB::White);
      Right.setNav(CRGB::White);
    break;

    case 56:
      navStrobeState = 0;
    default:
      Left.setNav(CRGB::Red);
      Right.setNav(CRGB::Green);
    break;
  }

  FastLED.show();
  navStrobeState++;
}

void strobe(int style) {
  CRGB right, left, body, tail;
  right = left = body = tail = CRGB::Black;
  switch(style) {
    case 1: // Rapid strobing all LEDS in unison
      switch(currentStep) {
        case 0:
          right = left = body = tail = CRGB::White;
        break;
        case 1:
          currentStep = -1;
        break;
      }
    break;

    case 2: // Alternate strobing of left and right wing
      tail = CRGB::White;
      if (currentStep < 10) {
        right = CRGB::White;
        body = CRGB::Blue;
      } else if (currentStep < 20) {
        left = CRGB::White;
        body = CRGB::Yellow;
      }
      if (currentStep == 19) {
        currentStep = -1;
      }
    break;

    case 3: // alternate double-blink strobing of left and right wing
      switch(currentStep) {
        case 0:
        case 2:
          right = CRGB::White;
        break;

        case 13:
        case 15:
          left = CRGB::White;
        break;

        case 25:
          currentStep = -1;
        break;
      }
    break;
  }

  fill_solid(Right.leds, Right.stopPoint, right);
  fill_solid(Left.leds, Left.stopPoint, left);
  fill_solid(Nose.leds, Nose.stopPoint, body);
  fill_solid(Fuse.leds, Fuse.stopPoint, body);
  fill_solid(Tail.leds, Tail.stopPoint, tail);

  currentStep++;
  FastLED.show();
}

void altitude(const CRGBPalette16& palette, double fake) {
  static double prevAlt;
  static int avgVSpeed[] = {0,0,0};


  int vSpeed;
  double currentAlt;

  currentAlt = getAltitude();
  
  if (fake != 0) {currentAlt = fake;}

  // take currentAlt, clamp and scale it to strip size * 2, so we can "overflow" and indicate when over the altitude limit
  uint8_t scaledWings = constrain(map(currentAlt, 0, MAX_ALTIMETER, 0, Right.stopPoint * 2), 0, Right.stopPoint * 2);
  uint8_t scaledFuse = constrain(map(currentAlt, 0, MAX_ALTIMETER, 0, Fuse.stopPoint * 2), 0, Fuse.stopPoint * 2);

  for (uint8_t i = 0; i < Right.stopPoint; i++) {
    CRGB color = CRGB::Black;
    if (i < scaledWings) {
      if (i < (scaledWings - Right.stopPoint)) {
        color = CRGB(255, 40, 0);
      } else {
        color = CRGB::White;
      }
    }
    Right.set(i, color);
    Left.set(i, color);
  }

  for (uint8_t i = 0; i < Fuse.stopPoint; i++) {
    CRGB color = CRGB::Black;
    if (i < scaledFuse) {
      if (i < (scaledFuse - Fuse.stopPoint)) {
        color = CRGB(255, 40, 0);
      } else {
        color = CRGB::White;
      }
    }
    Fuse.set(i, color);
  }

  // map vertical speed value to gradient palette
  int vspeedMap;
  avgVSpeed[0]=avgVSpeed[1];
  avgVSpeed[1]=avgVSpeed[2];
  avgVSpeed[2]=(currentAlt-prevAlt)*100; // *100 just gets things into int territory so we can work with it easier
  vSpeed = (avgVSpeed[0]+avgVSpeed[1]+avgVSpeed[2])/3;
  vSpeed = constrain(vSpeed, -interval, interval);

  vspeedMap = map(vSpeed, -interval, interval, 0, 240);

  fill_solid(Tail.leds, Tail.stopPoint, ColorFromPalette(palette, vspeedMap));

  prevAlt = currentAlt;

  FastLED.show();
}

enum {SteadyDim, Dimming, Brightening};

void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size, bool reset) {
  const CRGB colorDown = CRGB(1, 1, 1);
  const CRGB colorUp = CRGB(8, 8, 8);
  const CRGB colorMax = CRGB(128, 128, 128);
  const CRGB colorMin = CRGB(0, 0, 0);
  const uint8_t twinkleChance = 1;

  for (int i = 0; i < size; i++) {
    if (pixelState[i] == SteadyDim) {
      // if the pixel is steady dim, it has a random chance to start brightening
      if (random8() < twinkleChance) {
        pixelState[i] = Brightening;
      }
      if (reset) { // Reset all LEDs at start of show
        ledArray[i] = colorMin;
      }
    }

    if (pixelState[i] == Brightening) {
      // if it's brightening, once max, start dimming. otherwise keep going up
      if (ledArray[i] >= colorMax) {
        pixelState[i] = Dimming;
      } else {
        ledArray[i] += colorUp;
      }
    }

    if (pixelState[i] == Dimming) {
      // if dimming, once all the way dim, stop. otherwise, keep dimming
      if (ledArray[i] <= colorMin) {
        ledArray[i] = colorMin;
        pixelState[i] = SteadyDim;
      } else {
        ledArray[i] -= colorDown;
      }
    }
  }
}

void twinkle1(bool reset) {
  // arrays to hold the state of each LED of each strip
  static uint8_t* pixelStateRight = new uint8_t[Right.numLeds];
  static uint8_t* pixelStateLeft = new uint8_t[Left.numLeds];
  static uint8_t* pixelStateNose = new uint8_t[Nose.numLeds];
  static uint8_t* pixelStateFuse = new uint8_t[Fuse.numLeds];
  static uint8_t* pixelStateTail = new uint8_t[Tail.numLeds];

  if (reset) { // Reset everything at start of show
    memset(pixelStateRight, SteadyDim, sizeof(*pixelStateRight)*Right.numLeds);
    memset(pixelStateLeft, SteadyDim, sizeof(*pixelStateLeft)*Left.numLeds);
    memset(pixelStateNose, SteadyDim, sizeof(*pixelStateNose)*Nose.numLeds);
    memset(pixelStateFuse, SteadyDim, sizeof(*pixelStateFuse)*Fuse.numLeds);
    memset(pixelStateTail, SteadyDim, sizeof(*pixelStateTail)*Tail.numLeds);
  }

  doTwinkle1(Right.leds, pixelStateRight, Right.stopPoint, reset);
  doTwinkle1(Left.leds,  pixelStateLeft, Left.stopPoint, reset);
  doTwinkle1(Nose.leds,  pixelStateNose, Nose.stopPoint, reset);
  doTwinkle1(Fuse.leds,  pixelStateFuse, Fuse.stopPoint, reset);
  doTwinkle1(Tail.leds,  pixelStateTail, Tail.stopPoint, reset);

  FastLED.show();
}

void statusFlash(uint8_t status, uint8_t numFlashes, int delay_time) {
  CRGB color;
  switch (status) {
    case 'w':
      color = CRGB::White;
    break;

    case 1:
    case 'g':
      color = CRGB::Green;
    break;

    case 0:
    case 'r':
      color = CRGB::Red;
    break;
  }

  CRGB c;
  for (int j = 0; j < numFlashes*2; j++) {
    if ((j % 2) == 0) {
      c = color;
      digitalWrite(STATUS_LED, HIGH);
    } else {
      c = CRGB::Black;
      digitalWrite(STATUS_LED, LOW);
    }
    fill_solid(Right.leds, Right.stopPoint, c);
    fill_solid(Left.leds, Left.stopPoint, c);
    fill_solid(Nose.leds, Nose.stopPoint, c);
    fill_solid(Fuse.leds, Fuse.stopPoint, c);
    fill_solid(Tail.leds, Tail.stopPoint, c);
    FastLED.show();
    delay(delay_time);
  }
  digitalWrite(STATUS_LED, LOW);
}

void noSignal() {
  CRGB c;
  if (currentStep < 15) {
    c = CRGB::Red;
    digitalWrite(STATUS_LED, HIGH);
  } else if (currentStep < 30) {
    c = CRGB::Black;
    digitalWrite(STATUS_LED, LOW);
  } else {
    currentStep = -1;
  }

  fill_solid(Right.leds, Right.numLeds, c);
  fill_solid(Left.leds, Left.numLeds, c);
  fill_solid(Nose.leds, Nose.numLeds, c);
  fill_solid(Fuse.leds, Fuse.numLeds, c);
  fill_solid(Tail.leds, Tail.numLeds, c);
  FastLED.show();

  currentStep++;
}