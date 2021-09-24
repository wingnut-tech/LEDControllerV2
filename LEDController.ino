/*
 * Wingnut Tech LED Controller
 * 
 * VERSION: 1.3.0
 * DATE:    2021-09-24
 * 
 */

#include "layout.h"
#include "LEDController.h"

#include "src/LED.h"
#include "src/eeprom.h"
#include "src/shows.h"
#include "src/mode.h"
#include "src/pins.h"

#define FASTLED_INTERNAL // disables the FastLED version message that looks like an error
#include <FastLED.h>
#include <Adafruit_BMP280.h>


uint8_t numShows = NUM_SHOWS_WITH_ALTITUDE; // numShows becomes 1 less if no BMP280 module is installed
uint8_t numActiveShows = numShows; // how many actual active shows
uint8_t activeShowNumbers[NUM_SHOWS_WITH_ALTITUDE]; // our array of currently active show switchcase numbers

bool programMode = false; // are we in program mode?
int interval; // delay time between each "frame" of an animation
unsigned long currentMillis = millis();
int currentStep = 0; // this is just a global general-purpose counter variable that any show can use for whatever it needs

Adafruit_BMP280 bmp; // bmp280 module object
bool hasBMP280 = false; // did we detect a BMP280 module?
float basePressure; // gets initialized with ground level pressure on startup

Settings settings;

// set up the FastLED array variables
CRGB rightleds[WING_LEDS];
CRGB leftleds[WING_LEDS];
CRGB noseleds[NOSE_LEDS];
CRGB fuseleds[FUSE_LEDS];
CRGB tailleds[TAIL_LEDS];

// initialize the LED class objects for each string,
// passing in the actual array, the length of each string,
// and if they're reversed or not
LED Right = LED(rightleds, WING_LEDS, WING_REV);;
LED Left = LED(leftleds, WING_LEDS, WING_REV);
LED Nose = LED(noseleds, NOSE_LEDS, NOSE_REV);
LED Fuse = LED(fuseleds, FUSE_LEDS, FUSE_REV);
LED Tail = LED(tailleds, TAIL_LEDS, TAIL_REV);

/**
 * @brief Arduino first-run function
 * 
 */
void setup() {
  Serial.begin(115200);

  loadSettings();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  if (bmp.begin(0x76)) { // initialize the altitude pressure sensor with I2C address 0x76
    hasBMP280 = true;
    Serial.println(F("BMP280 module found."));
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X4,       /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_63);  /* Standby time. */

    basePressure = bmp.readPressure()/100; // this gets the current pressure at "ground level," so we can get relative altitude

    Serial.print(F("Base Pressure: "));
    Serial.println(basePressure);
  } else { // no BMP280 module installed
    Serial.println(F("No BMP280 module found. Disabling altitude() function."));
    numShows = NUM_SHOWS_WITH_ALTITUDE - 1;
  }

  pinMode(PROGRAM_CYCLE_BTN, INPUT_PULLUP);
  pinMode(PROGRAM_ENABLE_BTN, INPUT_PULLUP);
  pinMode(RC_PIN1, INPUT);
  pinMode(RC_PIN2, INPUT);

  // initialize FastLED arrays
  FastLED.addLeds<NEOPIXEL, RIGHT_PIN>(Right.leds, WING_LEDS);
  FastLED.addLeds<NEOPIXEL, LEFT_PIN>(Left.leds, WING_LEDS);
  FastLED.addLeds<NEOPIXEL, FUSE_PIN>(Fuse.leds, FUSE_LEDS);
  FastLED.addLeds<NEOPIXEL, NOSE_PIN>(Nose.leds, NOSE_LEDS);
  FastLED.addLeds<NEOPIXEL, TAIL_PIN>(Tail.leds, TAIL_LEDS);

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

  FastLED.setBrightness(MAX_BRIGHTNESS);
}

/**
 * @brief Arduino main loop
 * 
 */
void loop() {
  static bool firstrun = true;
  static unsigned long prevMillis = 0; // keeps track of last millis value for regular show timing
  static unsigned long prevNavMillis = 0; // keeps track of last millis value for the navlights

  if (firstrun) {
    setInitPattern(); // Set the LED strings to their boot-up configuration
    firstrun = false;
  }

  // The timing control for calling each "frame" of the different animations
  currentMillis = millis();
  if (currentMillis - prevMillis > interval) {
    prevMillis = currentMillis;
    stepShow();
  }

  if (settings.navlights) { // navlights if enabled
    if (currentMillis - prevNavMillis > 30) {
      prevNavMillis = currentMillis;
        navLights();
    }
  }

  if (programMode) { // we are in program mode where the user can enable/disable programs
    program();
  } else { // we are not in program mode. Read signal from receiver and run through programs normally.
    normal();
  }
}
