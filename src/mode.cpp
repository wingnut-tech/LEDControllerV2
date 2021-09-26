#include "../LEDController.h"
#include "mode.h"

#include "shows.h"
#include "eeprom.h"
#include "pins.h"

extern bool programMode;
extern bool hasBMP280;
extern uint8_t currentShow;
extern uint8_t numShows;
extern uint8_t numActiveShows;
extern long currentMillis;

extern Settings settings;

unsigned long progMillis = 0; // keeps track of last millis value for button presses
int programModeCounter = 0;
int enableCounter = 0;

void program() {
  // Are we exiting program mode?
  if (digitalRead(PROGRAM_CYCLE_BTN) == LOW) { // we're in program mode. is the Program/Cycle button pressed?
    programModeCounter = programModeCounter + (currentMillis - progMillis); // increment the counter by how many milliseconds have passed
    if (programModeCounter > 3000) { // Has the button been held down for 3 seconds?
      programMode = false;
      Serial.println(F("Exiting program mode"));
      // store current program values into eeprom
      saveSettings();
      programModeCounter = 0;
      statusFlash('w'); //strobe the leds to indicate leaving program mode
    }
  } else { // button not pressed
    if (programModeCounter > 0 && programModeCounter < 1000) { // a momentary press to cycle to the next program
      currentShow++;
      if (currentShow == numShows) {currentShow = 0;}
    }
    programModeCounter = 0;
  }
  
  if (digitalRead(PROGRAM_ENABLE_BTN) == LOW) { // we're in program mode. is the Program Enable/Disable button pressed?
    enableCounter = enableCounter + (currentMillis - progMillis); // increment the counter by how many milliseconds have passed
  } else { // button not pressed
    if (enableCounter > 0 && enableCounter < 1000) { // momentary press to toggle the current show
      // toggle the state of the current program on/off
      if (settings.enabledShows[currentShow] == true) {
        settings.enabledShows[currentShow] = false;
        Serial.println(F("disabled"));
      } else {
        settings.enabledShows[currentShow] = true;
        Serial.println(F("enabled"));
      }
      statusFlash(settings.enabledShows[currentShow]);
    }
    enableCounter = 0;
  }
  progMillis = currentMillis;
}

void normal() {
  static unsigned long prevAutoMillis = 0;
  static uint8_t rcInputPort = 0; // which RC input port is plugged in? 0 watches both 1 and 2, until either one gets a valid signal. then this gets set to that number
  static int pwmLow = 1100;
  static int pwmHigh = 1900;
  
  int currentCh1 = 0;  // Receiver Channel PPM value
  int currentCh2 = 0;  // Receiver Channel PPM value

#ifndef TESTMODE // if TESTMODE isn't defined, read the RC signal
  if (rcInputPort == 0 || rcInputPort == 1) { // if rcInputPort == 0, check both rc input pins until we get a valid signal on one
    currentCh1 = pulseIn(RC_PIN1, HIGH, 25000);  // (Pin, State, Timeout)
    if (currentCh1 > 700 && currentCh1 < 2400) { // do we have a valid signal?
      if (currentCh1 > pwmHigh) {
        pwmHigh = currentCh1;
      }
      if (currentCh1 < pwmLow) {
        pwmLow = currentCh1;
      }
      if (rcInputPort == 0) {
        rcInputPort = 1; // if we were on "either" port mode, switch it to 1
        statusFlash('w', 1, 300); // flash white once for RC input 1
        statusFlash(hasBMP280, 1, 300); // indicate BMP280 module present
      }
      currentShow = map(currentCh1, pwmLow, pwmHigh+1, 0, numActiveShows); // map the input. this logic should cause slices to be equal, without going over numActiveShows
    }
  }
  if (rcInputPort == 0 || rcInputPort == 2) { // RC_PIN2 is our 2-position-switch autoscroll mode
    currentCh2 = pulseIn(RC_PIN2, HIGH, 25000);  // (Pin, State, Timeout)
    if (currentCh2 > 700 && currentCh2 < 2400) { // valid signal?
      if (rcInputPort == 0) {
        rcInputPort = 2; // if we were on "either" port mode, switch it to 2
        statusFlash('w', 2, 300); // flash white twice for RC input 2
        statusFlash(hasBMP280, 1, 300); // indicate BMP280 module present
      }
      if (currentCh2 > 1500) {
        // switch is "up" (above 1500), auto-scroll through shows
        if (currentMillis - prevAutoMillis > 2000) { // auto-advance after 2 seconds
          currentShow += 1;
          prevAutoMillis = currentMillis;
        }
      } else { // switch is "down" (below 1500), stop autoscrolling, reset timer
        prevAutoMillis = currentMillis - 1995; // this keeps the the auto-advance timer constantly "primed", so when flipping the switch again, it advances right away
      }
    }
  }
  if (rcInputPort == 0) {
    statusFlash('r', 1, 300); // flash red to indicate no signal
  }
  currentShow = currentShow % numActiveShows; // keep currentShow within the limits of our active shows
#else // TESTMODE is defined, force specified show number
  delayMicroseconds(20000); // pulseIn does cause a delay itself, so this helps keep things somewhat similar
  currentShow = TESTMODE;
#endif
  
  // Are we entering program mode?
  if (digitalRead(PROGRAM_CYCLE_BTN) == LOW) { // Is the Program button pressed?
    programModeCounter = programModeCounter + (currentMillis - progMillis); // increment the counter by how many milliseconds have passed
    if (programModeCounter > 3000) { // Has the button been held down for 3 seconds?
      programMode = true;
      programModeCounter = 0;
      Serial.println(F("Entering program mode"));
      statusFlash('w'); //strobe the leds to indicate entering program mode
      currentShow = 0;
      statusFlash(settings.enabledShows[currentShow]);
    }
  } else if (digitalRead(PROGRAM_ENABLE_BTN) == LOW) { // Program button not pressed, but is Enable/Disable button pressed?
    programModeCounter = programModeCounter + (currentMillis - progMillis);
    if (programModeCounter > 3000) {
      // toggle the navlights on/off
      if (settings.navlights == true) {settings.navlights = false;}
      else {settings.navlights = true;}
      saveSettings();
      programModeCounter = 0;
    }
  } else { // no buttons are being pressed
    programModeCounter = 0;
  }
  progMillis = currentMillis;
}
