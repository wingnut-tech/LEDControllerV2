#include "serial.h"

#include "../LEDController.h"
#include "bmx280.h"
#include "eeprom.h"
#include "LED.h"
#include "mode.h"
#include "visualize.h"


void menu() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (programMode) {
      switch (input) {
      case 'p':
        exitProgramMode();
        printHelp();
        break;
      case ']':
        goNextShow();
        break;
      case '[':
        goPrevShow();
        break;
      case ' ':
        toggleShow();
        break;
      default:
        printAll();
        break;
      }
    } else {
      switch (input) {
      case 'p':
        enterProgramMode();
        printHelp();
        break;
      case 'n':
        toggleNavLights();
        break;
#ifdef VISUALIZER
      case 'V':
        visualize();
        break;
#endif
      default:
        printAll();
        break;
      }
    }
  }
}


void printHelp() {
#ifdef ARDUINO_ARCH_RP2040
  if (programMode) {
    Serial.println();
    Serial.println("[     - Previous show");
    Serial.println("]     - Next show");
    Serial.println("space - Toggle current show");
    Serial.println("p     - Exit program mode");
    Serial.println();
  } else {
    Serial.println();
    Serial.println("p     - Program mode");
    Serial.println("n     - Toggle navlights");
    Serial.println();
  }
#endif
}


void printAll() {
  printSettings();
  printLayout();
  printBrightness();
  printBMX();
  printHelp();
}