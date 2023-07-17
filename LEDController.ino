/*
 * Wingnut Tech LED Controller
 * 
 * VERSION: v2.0.0
 * DATE:    2023-07-17
 * 
 */

#include "LEDController.h"

#include "src/LED.h"
#include "src/eeprom.h"
#include "src/shows.h"
#include "src/mode.h"
#include "src/pins.h"
#include "src/bmx280.h"
#include "src/serial.h"
#include "src/input.h"


void setup() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  pinMode(PROGRAM_CYCLE_BTN, INPUT_PULLUP);
  pinMode(PROGRAM_ENABLE_BTN, INPUT_PULLUP);

#ifdef ARDUINO_ARCH_RP2040
  pinMode(RC_PIN1, INPUT_PULLDOWN);
  pinMode(RC_PIN2, INPUT_PULLDOWN);
#else
  pinMode(RC_PIN1, INPUT);
  pinMode(RC_PIN2, INPUT);
#endif
  
  Serial.begin(115200);

  initEEPROM();
  loadSettings();
  loadLayout();
  setupLeds();
  updateNavlights();
  initBMX();

  updateActiveShows();

  printAll();
}


void loop() {
  currentMode();

  menu();
}


// Second core setup/loop for RP2040

void setup1() {
}

void loop1() {
  readInputs();
}
