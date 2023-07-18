#include "mode.h"

#include "../LEDController.h"
#include "LED.h"
#include "shows.h"
#include "eeprom.h"
#include "pins.h"
#include "button.h"
#include "input.h"

bool programMode = false;
bool visualizeMode = false;
bool haveSignal = false;

static Button enableBtn = Button(PROGRAM_ENABLE_BTN);
static Button cycleBtn = Button(PROGRAM_CYCLE_BTN);

static uint16_t pwmLow = 1100;
static uint16_t pwmHigh = 1900;
static uint8_t showStep = (pwmHigh - pwmLow) / numActiveShows;
static uint8_t showThresh = showStep >> 2;


void currentMode() {
  static unsigned long prevMillis = 0;
  static unsigned long prevNavMillis = 0;
  
  enableBtn.loop();
  cycleBtn.loop();

  if (programMode) {
    program();
  } else {
    normal();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis > interval) {
    prevMillis = currentMillis;
    stepShow();
  }

  if (currentMillis - prevNavMillis > 30) {
    navLights();
    prevNavMillis = currentMillis;
  }
}


void program() {
  int cyclePress = cycleBtn.getPress();
  if (cyclePress == LONG) {
    exitProgramMode();
  } else if (cyclePress == SHORT) {
    goNextShow();
  }

  int enablePress = enableBtn.getPress();
  if (enablePress == SHORT) {
    toggleShow();
  }
}


void normal() {
#ifdef TESTMODE // TESTMODE is defined, force specified show number
  currentShow = TESTMODE;
#else // TESTMODE isn't defined, read the RC signal
  static unsigned long prevAutoMillis = 0;

#ifndef ARDUINO_ARCH_RP2040
  readInputs(); // read RC input here if on atmega chip, rp2040 reads on second core
#endif

  uint16_t input;
  if (currentCh == CH1) {
    input = currentCh1;
    
    if (input < pwmLow || input > pwmHigh) {
      pwmLow = min(input, pwmLow);
      pwmHigh = max(input, pwmHigh);
      updateStep();
    }

    if ((currentShow > 0) && (currentCh1 < (pwmLow + (showStep * currentShow) - showThresh))) { 
      currentShow--;
    } else if ((currentShow < (numActiveShows-1)) && (currentCh1 > (pwmLow + (showStep * (currentShow+1)) + showThresh))) {
      currentShow++;
    }
  } else if (currentCh == CH2) {
    input = currentCh2;
    unsigned long currentMillis = millis();
    if (input > 1500) {
      // switch is up (above 1500), auto-scroll through shows
      if (currentMillis - prevAutoMillis > 2000) { // auto-advance after 2 seconds
        currentShow += 1;
        prevAutoMillis = currentMillis;
      }
    } else { // switch is down (below 1500), stop autoscrolling, reset timer
      prevAutoMillis = currentMillis - 1995; // keep auto-advance timer primed, so when flipping the switch again, it advances right away
    }
  }

  currentShow = currentShow % numActiveShows; // keep currentShow within the limits of our active shows
#endif
  
  // process button presses
  int cyclePress = cycleBtn.getPress();
  int enablePress = enableBtn.getPress();
  if (cyclePress == LONG) {
    enterProgramMode();
  } else if (enablePress == LONG) {
    toggleNavLights();
  }
}


void enterProgramMode() {
  programMode = true;
  Serial.println(F("Entering program mode"));
  statusFlash('w'); //strobe the leds to indicate entering program mode
  currentShow = 0;
  statusFlash(settings.enabledShows[currentShow]);
}


void exitProgramMode() {
  programMode = false;
  Serial.println(F("Exiting program mode"));
  saveSettings();
  updateActiveShows();
  statusFlash('w'); //strobe the leds to indicate leaving program mode
}

void goNextShow() {
  currentShow = (currentShow + 1) % numShows;
}

void goPrevShow() {
  currentShow -= 1;
  if (currentShow > numShows) {
    currentShow = 0;
  }
}


void toggleShow() {
  if (settings.enabledShows[currentShow] == true) {
    settings.enabledShows[currentShow] = false;
    Serial.println(F("disabled"));
  } else {
    settings.enabledShows[currentShow] = true;
    Serial.println(F("enabled"));
  }
  statusFlash(settings.enabledShows[currentShow]);
}


void updateStep() {
  showStep = (pwmHigh - pwmLow) / numActiveShows;
  showThresh = showStep >> 2;
}


void updateActiveShows() {
  uint8_t active = 0;

  for (int i = 0; i < numShows; i++) {
    if (settings.enabledShows[i]) {
      activeShowNumbers[active] = i;
      active++;
    }
  }
  numActiveShows = active;
  updateStep();
}