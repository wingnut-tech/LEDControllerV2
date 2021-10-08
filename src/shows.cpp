#include "../LEDController.h"
#include "../layout.h"

#include "LED.h"
#include "patterns.h"
#include "eeprom.h"
#include "shows.h"

#define FASTLED_INTERNAL
#include <FastLED.h>
#include <Adafruit_BMP280.h>

extern LED Right;
extern LED Left;
extern LED Nose;
extern LED Fuse;
extern LED Tail;

extern uint8_t activeShowNumbers[];
extern int interval;
extern int currentStep;
extern bool programMode;
extern Adafruit_BMP280 bmp;
extern float basePressure;

extern Settings settings;

uint8_t currentShow = 0;
uint8_t prevShow;

const uint8_t maxLeds = max(WING_LEDS, max((NOSE_LEDS+FUSE_LEDS), TAIL_LEDS));

void stepShow() {
  #define caseshow(x,y,i) case x: y; interval=i; break; // macro for switchcases with a built-in break
  
  if (currentShow != prevShow) { // did we just switch to a new show?
    Serial.print(F("Current Show: "));
    Serial.println(currentShow);
    currentStep = 0; // reset the global general-purpose counter
    setColor(CRGB::Black);
    if (programMode) { // if we're in program mode and just switched, indicate show status
      statusFlash(settings.enabledShows[currentShow]); //flash all LEDs red/green to indicate current show status
    }
  }

  int switchShow; // actual show to select in the switchcase
  if (programMode) { // if we're in program mode, select from all shows
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
    caseshow(11, twinkle1(), 10); // twinkle effect
    caseshow(12, strobe(3), 50); // Realistic double strobe alternating between wings
    caseshow(13, strobe(2), 50); // Realistic landing-light style alternating between wings
    caseshow(14, strobe(1), 50); // unrealistic rapid strobe of all non-nav leds, good locator/identifier. also might cause seizures
    caseshow(15, chase(CRGB::White, CRGB::Black, 50, 80, 35, 80), 10); // "chase" effect, with a white streak on a black background
    caseshow(16, chase(CRGB::Red, CRGB::Black, 30, 50, 30, 50, true), 10); // Night Rider/Cylon style red beam scanning back and forth
    caseshow(17, juggle(4, 8), 10); // multiple unique "pulses" of light bouncing back and forth, all with different colors
    caseshow(18, animateColor(USA, 4, 1), 20); // sweeps a palette across the whole plane

    // altitude needs to be the last show so we can disable it if no BMP280 module is installed
    caseshow(19, altitude(variometer), 100); // second parameter can be to another number for "fake" altitude
  }
  prevShow = currentShow;
}

void setColor(const CRGB& color) {
  fill_solid(Right.leds, Right.stopPoint, color);
  fill_solid(Left.leds, Left.stopPoint, color);
  fill_solid(Nose.leds, NOSE_LEDS, color);
  fill_solid(Fuse.leds, FUSE_LEDS, color);
  fill_solid(Tail.leds, TAIL_LEDS, color);
  FastLED.show();
}

void setColor(const CRGBPalette16& palette) {
  for (int i = 0; i < maxLeds; i++) {
    // range of 0-240 is used in the map() function due to how the FastLED ColorFromPalette() function works.
    // 240 is actually the correct "wrap-around" point
    Right.set(i, ColorFromPalette(palette, map(i, 0, Right.stopPoint, 0, 240)));
    Left.set(i, ColorFromPalette(palette, map(i, 0, Left.stopPoint, 0, 240)));
    Tail.set(i, ColorFromPalette(palette, map(i, 0, TAIL_LEDS, 0, 240)));
    if (NOSE_FUSE_JOINED) {
      setNoseFuse(i, ColorFromPalette(palette, map(i, 0, NOSE_LEDS+FUSE_LEDS, 0, 240)));
    } else {
      Nose.set(i, ColorFromPalette(palette, map(i, 0, NOSE_LEDS, 0, 240)));
      Fuse.set(i, ColorFromPalette(palette, map(i, 0, FUSE_LEDS, 0, 240)));
    }
  }
  FastLED.show();
}

CRGB LetterToColor(char letter) {
  CRGB color;
  switch (letter) {
    case 'r': color = CRGB::Red;
              break;
    case 'g': color = CRGB::Green;
              break;
    case 'b': color = CRGB::Blue;
              break;
    case 'w': color = CRGB::White;
              break;
    case 'a': color = CRGB::AntiqueWhite;
              break;
    case 'o': color = CRGB::Black; // o = off
              break;
  }
  return color;
}

void setPattern(char pattern[]) {
  for (int i = 0; i < maxLeds; i++) {
    Right.set(i, LetterToColor(pattern[i]));
    Left.set(i, LetterToColor(pattern[i]));
  }
  FastLED.show();
}

void setNoseFuse(uint8_t led, const CRGB& color, bool addor=false) {
  if (led < NOSE_LEDS) { // less than NOSE_LEDS, set the nose
    if (addor) {
      Nose.addor(led, color);
    } else {
      Nose.set(led, color);
    }
  } else { // greater than NOSE_LEDS, set fuse (minus NOSE_LEDS)
    if (addor) {
      Fuse.addor(led-NOSE_LEDS, color);
    } else {
      Fuse.set(led-NOSE_LEDS, color);
    }
  }
}

void setBothWings(uint8_t led, const CRGB& color, bool addor=false) {
  if (led < Left.stopPoint) { // less than left size, set left wing, but "reversed" (start at outside)
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
    // scale to 240 again because that's the correct "wrap" point for ColorFromPalette()
    CRGB color = ColorFromPalette(palette, scale8(triwave8((i * ledOffset) + currentStep), 240));
    Right.set(i, color);
    Left.set(i, color);
    Tail.set(i, color);
    if (NOSE_FUSE_JOINED) {
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
    if (NOSE_FUSE_JOINED) {
      setNoseFuse(i, color);
    } else {
      Nose.set(i, color);
      Fuse.set(i, color);
    }
  }
  currentStep++;
  FastLED.show();
}

void chase(const CRGB& color1, const CRGB& color2, uint8_t speedWing, uint8_t speedNose, uint8_t speedFuse, uint8_t speedTail, bool cylon=false) {
  // fade out the whole string to get a nice fading "trail"
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
    setBothWings(scale8(triwave8(beat8(speedWing)), (Right.stopPoint+Left.stopPoint)-1), color1);
    Tail.set(scale8(triwave8(beat8(speedTail)), TAIL_LEDS-1), color1);
    if (NOSE_FUSE_JOINED) {
      setNoseFuse(scale8(triwave8(beat8(speedFuse)), (NOSE_LEDS+FUSE_LEDS)-1), color1);
    } else {
      Nose.set(scale8(triwave8(beat8(speedNose)), NOSE_LEDS-1), color1);
      Fuse.set(scale8(triwave8(beat8(speedFuse)), FUSE_LEDS-1), color1);
    }
  } else {
    // chase just goes "out" in the same directions
    Right.set(scale8(beat8(speedWing), Right.stopPoint-1), color1);
    Left.set(scale8(beat8(speedWing), Left.stopPoint-1), color1);
    Tail.set(scale8(beat8(speedTail), TAIL_LEDS-1), color1);
    if (NOSE_FUSE_JOINED) {
      setNoseFuse(scale8(beat8(speedFuse), (NOSE_LEDS+FUSE_LEDS)-1), color1);
    } else {
      Nose.set(scale8(beat8(speedNose), NOSE_LEDS-1), color1);
      Fuse.set(scale8(beat8(speedFuse), FUSE_LEDS-1), color1);
    }
  }

  FastLED.show();
}

void juggle(uint8_t numPulses, uint8_t speed) {
  uint8_t spread = 256 / numPulses;

  // fade out the whole string to get a nice fading "trail"
  Right.nscale8(192);
  Left.nscale8(192);
  Fuse.nscale8(192);
  Nose.nscale8(192);
  Tail.nscale8(192);

  for (uint8_t i = 0; i < numPulses; i++) {
    // use addor on everything, so colors add when overlapping
    setBothWings(beatsin8(i+speed, 0, (Right.stopPoint+Left.stopPoint)-1), CHSV(i*spread + beat8(1), 200, 255), true);
    Tail.addor(beatsin8(i+speed, 0, TAIL_LEDS-1), CHSV(i*spread + beat8(1), 200, 255));
    if (NOSE_FUSE_JOINED) {
      setNoseFuse(beatsin8(i+speed, 0, (NOSE_LEDS+FUSE_LEDS)-1), CHSV(i*spread + beat8(1), 200, 255), true);
    } else {
      Nose.addor(beatsin8(i+speed, 0, NOSE_LEDS-1), CHSV(i*spread + beat8(1), 200, 255));
      Fuse.addor(beatsin8(i+speed, 0, FUSE_LEDS-1), CHSV(i*spread + beat8(1), 200, 255));
    }
  }

  FastLED.show();
}

void navLights() {
static uint8_t navStrobeState = 0;
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
      // red/green
      Left.setNav(CRGB::Red);
      Right.setNav(CRGB::Green);
    break;
  }
  FastLED.show();
  navStrobeState++;
}

void strobe(int style) {
  switch(style) {
    case 1: // Rapid strobing all LEDS in unison
      CRGB color;
      switch(currentStep) {
        case 0:
            color = CRGB::White;
        break;
        case 1:
            color = CRGB::Black;
          currentStep = -1;
        break;
      }
      fill_solid(Right.leds, Right.stopPoint, color);
      fill_solid(Left.leds, Left.stopPoint, color);
      fill_solid(Nose.leds, NOSE_LEDS, color);
      fill_solid(Fuse.leds, FUSE_LEDS, color);
      fill_solid(Tail.leds, TAIL_LEDS, color);
    break;

    case 2: // Alternate strobing of left and right wing
      CRGB right, left, body;
      switch (currentStep) {
        case 0:
            right = CRGB::White;
            left = CRGB::Black;
            body = CRGB::Blue;
        break;
        case 10:
            right = CRGB::Black;
            left = CRGB::White;
            body = CRGB::Yellow;
        break;
        case 19:
          currentStep = -1;
        break;
      }
      fill_solid(Right.leds, Right.stopPoint, right);
      fill_solid(Left.leds, Left.stopPoint, left);
      fill_solid(Nose.leds, NOSE_LEDS, body);
      fill_solid(Fuse.leds, FUSE_LEDS, body);
      fill_solid(Tail.leds, TAIL_LEDS, CRGB::White);
    break;
    case 3: // alternate double-blink strobing of left and right wing
      switch(currentStep) {
        case 0:
        case 2:
          fill_solid(Right.leds, Right.stopPoint, CRGB::White);
          fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;

        case 13:
        case 15:
          fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
          fill_solid(Left.leds, Left.stopPoint, CRGB::White);
        break;

        case 25:
          currentStep = -1;
        break;

        default:
          fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
          fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
      }
    break;
  }

  currentStep++;
  FastLED.show();
}

void altitude(const CRGBPalette16& palette, double fake=0) {
#define METRIC_CONVERSION 3.3; // 3.3 to convert meters to feet. 1 for meters.

  static double prevAlt;
  static int avgVSpeed[] = {0,0,0};


  int vSpeed;
  double currentAlt;

  currentAlt = bmp.readAltitude(basePressure)*METRIC_CONVERSION;
  
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

  fill_solid(Tail.leds, TAIL_LEDS, ColorFromPalette(palette, vspeedMap));

  #ifdef DEBUG_ALTITUDE
  Serial.print(F("Current relative altitude:  "));
  Serial.print(currentAlt);
  Serial.print(F("\t\tVSpeed: "));
  Serial.print(vSpeed);
  Serial.print(F("\tVSpeedMap: "));
  Serial.println(vspeedMap);
  #endif

  prevAlt = currentAlt;

  FastLED.show();
}

enum {SteadyDim, Dimming, Brightening};

void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size) {
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
      if (prevShow != currentShow) { // Reset all LEDs at start of show
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

void twinkle1() {
  // arrays to hold the "state" of each LED of each strip
  static uint8_t pixelStateRight[WING_LEDS];
  static uint8_t pixelStateLeft[WING_LEDS];
  static uint8_t pixelStateNose[NOSE_LEDS];
  static uint8_t pixelStateFuse[FUSE_LEDS];
  static uint8_t pixelStateTail[TAIL_LEDS];

  if (prevShow != currentShow) { // Reset everything at start of show
    memset(pixelStateRight, SteadyDim, sizeof(pixelStateRight));
    memset(pixelStateLeft, SteadyDim, sizeof(pixelStateLeft));
    memset(pixelStateNose, SteadyDim, sizeof(pixelStateNose));
    memset(pixelStateFuse, SteadyDim, sizeof(pixelStateFuse));
    memset(pixelStateTail, SteadyDim, sizeof(pixelStateTail));
  }

  doTwinkle1(Right.leds, pixelStateRight, Right.stopPoint);
  doTwinkle1(Left.leds,  pixelStateLeft, Left.stopPoint);
  doTwinkle1(Nose.leds,  pixelStateNose, NOSE_LEDS);
  doTwinkle1(Fuse.leds,  pixelStateFuse, FUSE_LEDS);
  doTwinkle1(Tail.leds,  pixelStateTail, TAIL_LEDS);

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
  for (int j = 0; j < numFlashes; j++) {
    fill_solid(Right.leds, Right.stopPoint, color);
    fill_solid(Left.leds, Left.stopPoint, color);
    fill_solid(Nose.leds, NOSE_LEDS, color);
    fill_solid(Fuse.leds, FUSE_LEDS, color);
    fill_solid(Tail.leds, TAIL_LEDS, color);
    digitalWrite(LED_BUILTIN, HIGH);
    FastLED.show();
    delay(delay_time);

    fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
    fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
    fill_solid(Nose.leds, NOSE_LEDS, CRGB::Black);
    fill_solid(Fuse.leds, FUSE_LEDS, CRGB::Black);
    fill_solid(Tail.leds, TAIL_LEDS, CRGB::Black);
    digitalWrite(LED_BUILTIN, LOW);
    FastLED.show();
    delay(delay_time);
  }
  digitalWrite(LED_BUILTIN, LOW);
}
