#include "../LEDController.h"

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
  #define caseshow(x,y) case x: y; break; // macro for switchcases with a built-in break
  
  if (currentShow != prevShow) { // did we just switch to a new show?
    Serial.print(F("Current Show: "));
    Serial.println(currentShow);
    currentStep = 0; // reset the global general-purpose counter
    blank();
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
    caseshow(0,  blank()); // all off except for NAV lights, if enabled
    caseshow(1,  colorWave1(10, 10)); // regular rainbow
    caseshow(2,  colorWave1(0, 10)); // zero led offset, so the whole plane is a solid color rainbow
    caseshow(3,  setColor(CRGB::Red)); // whole plane solid color
    caseshow(4,  setColor(CRGB::Orange));
    caseshow(5,  setColor(CRGB::Yellow));
    caseshow(6,  setColor(CRGB::Green));
    caseshow(7,  setColor(CRGB::Blue));
    caseshow(8,  setColor(CRGB::Indigo));
    caseshow(9,  setColor(CRGB::DarkCyan));
    caseshow(10, setColor(CRGB::White));
    caseshow(11, twinkle1()); // twinkle effect
    caseshow(12, strobe(3)); // Realistic double strobe alternating between wings
    caseshow(13, strobe(2)); // Realistic landing-light style alternating between wings
    caseshow(14, strobe(1)); // unrealistic rapid strobe of all non-nav leds, good locator/identifier. also might cause seizures
    caseshow(15, chase(CRGB::White, CRGB::Black, 50, 80, 35, 80)); // "chase" effect, with a white streak on a black background
    caseshow(16, chase(CRGB::Red, CRGB::Black, 30, 50, 30, 50, true)); // Night Rider/Cylon style red beam scanning back and forth
    caseshow(17, juggle(4, 8)); // multiple unique "pulses" of light bouncing back and forth, all with different colors
    caseshow(18, animateColor(USA, 4, 1)); // sweeps a palette across the whole plane

    //altitude needs to be the last show so we can disable it if no BMP280 module is installed
    caseshow(19, altitude(0, variometer)); // first parameter is for testing. 0 for real live data, set to another number for "fake" altitude
  }
  prevShow = currentShow;
}

void blank() {
  setColor(CRGB::Black);
  interval = 50;
  FastLED.show();
}

void setColor(const CRGB& color) {
  fill_solid(Right.leds, Right.stopPoint, color);
  fill_solid(Left.leds, Left.stopPoint, color);
  fill_solid(Nose.leds, NOSE_LEDS, color);
  fill_solid(Fuse.leds, FUSE_LEDS, color);
  fill_solid(Tail.leds, TAIL_LEDS, color);
  interval = 50;
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
  interval = 20;
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
  interval = 20;
  FastLED.show();
}

void setInitPattern() {
  for (int i = 0; i < WING_LEDS; i++) {
    Right.set(i, LetterToColor(init_rightwing[i]));
  }
  
  for (int i = 0; i < WING_LEDS; i++) {
    Left.set(i, LetterToColor(init_leftwing[i]));
  }
  
  for (int i = 0; i < NOSE_LEDS; i++) {
    Nose.set(i, LetterToColor(init_nose[i]));
  }
  
  for (int i = 0; i < FUSE_LEDS; i++) {
    Fuse.set(i, LetterToColor(init_fuse[i]));
  }
  
  for (int i = 0; i < TAIL_LEDS; i++) {
    Tail.set(i, LetterToColor(init_tail[i]));
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
  interval = 20;
  FastLED.show();
}

void colorWave1(uint8_t ledOffset, uint8_t l_interval) {
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
  interval = l_interval;
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

  interval = 10;
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

  interval = 10;
  FastLED.show();
}

void navLights() {
static uint8_t navStrobeState = 0;
  switch(navStrobeState) {
    case 0:
      // red/green
      Left.setNav(CRGB::Red);
      Right.setNav(CRGB::Green);
      break;
    case 50:
      // strobe 1
      Left.setNav(CRGB::White);
      Right.setNav(CRGB::White);
      break;
    case 52:
      // back to red/green
      Left.setNav(CRGB::Red);
      Right.setNav(CRGB::Green);
      break;
    case 54:
      // strobe 2
      Left.setNav(CRGB::White);
      Right.setNav(CRGB::White);
      break;
    case 56:
      // red/green again
      Left.setNav(CRGB::Red);
      Right.setNav(CRGB::Green);
      navStrobeState = 0;
      break;
  }
  FastLED.show();
  navStrobeState++;
}

void strobe(int style) {
  switch(style) {
    case 1: // Rapid strobing all LEDS in unison
      switch(currentStep) {
        case 0:
            fill_solid(Right.leds, Right.stopPoint, CRGB::White);
            fill_solid(Left.leds, Left.stopPoint, CRGB::White);
            fill_solid(Nose.leds, NOSE_LEDS, CRGB::White);
            fill_solid(Fuse.leds, FUSE_LEDS, CRGB::White);
            fill_solid(Tail.leds, TAIL_LEDS, CRGB::White);
        break;
        case 1:
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
            fill_solid(Nose.leds, NOSE_LEDS, CRGB::Black);
            fill_solid(Fuse.leds, FUSE_LEDS, CRGB::Black);
            fill_solid(Tail.leds, TAIL_LEDS, CRGB::Black);
          currentStep = -1;
        break;
      }
    break;

    case 2: // Alternate strobing of left and right wing
      switch (currentStep) {
        case 0:
            fill_solid(Right.leds, Right.stopPoint, CRGB::White);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
            fill_solid(Nose.leds, NOSE_LEDS, CRGB::Blue);
            fill_solid(Fuse.leds, FUSE_LEDS, CRGB::Blue);
            fill_solid(Tail.leds, TAIL_LEDS, CRGB::White);
        break;
        case 10:
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::White);
            fill_solid(Nose.leds, NOSE_LEDS, CRGB::Yellow);
            fill_solid(Fuse.leds, FUSE_LEDS, CRGB::Yellow);
            fill_solid(Tail.leds, TAIL_LEDS, CRGB::White);
        break;
        case 19:
          currentStep = -1;
        break;
      }
    break;
    case 3: // alternate double-blink strobing of left and right wing
      switch(currentStep) {
        case 0: // Right wing on for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::White);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 1: // Both wings off for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 2: // Right wing on for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::White);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 3: // Both wings off for 500ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 13: // Left wing on for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::White);
        break;
        case 14: // Both wings off for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 15: // Left wing on for 50ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::White);
        break;
        case 16: // Both wings off for 500ms
            fill_solid(Right.leds, Right.stopPoint, CRGB::Black);
            fill_solid(Left.leds, Left.stopPoint, CRGB::Black);
        break;
        case 25:
          currentStep = -1;
        break;
    }
    break;
  }

  interval = 50;
  currentStep++;
  FastLED.show();
}

void altitude(double fake, const CRGBPalette16& palette) {
  static double prevAlt;
  static int avgVSpeed[] = {0,0,0,0};

  interval = 100; // we're also going to use interval as a "time delta" to base the vspeed off of

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

  Serial.print(F("Current relative altitude:  "));
  Serial.print(currentAlt);
  Serial.print(F("\t\tVSpeed: "));
  Serial.print(vSpeed);
  Serial.print(F("\tVSpeedMap: "));
  Serial.println(vspeedMap);
  
  prevAlt = currentAlt;

  FastLED.show();
}

void doTwinkle1(CRGB * ledArray, uint8_t * pixelState, uint8_t size) {
  const CRGB colorDown = CRGB(1, 1, 1);
  const CRGB colorUp = CRGB(8, 8, 8);
  const CRGB colorMax = CRGB(128, 128, 128);
  const CRGB colorMin = CRGB(0, 0, 0);
  const uint8_t twinkleChance = 1;

  for (int i = 0; i < size; i++) {
    if (pixelState[i] == SteadyDim) {
      // if the pixel is steady dim, it has a random change to start brightening
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

  interval = 10;
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
