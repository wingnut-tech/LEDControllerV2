#include "eeprom.h"

#include "LED.h"
#include "shows.h"

#include <EEPROM.h>

Settings settings;
Layout layout;

#ifndef ARDUINO_ARCH_RP2040
const char enabled[][9] = {"disabled", " enabled"};
const char tf[][6] = {"false", " true"};
#endif

void initEEPROM() {
#ifdef ARDUINO_ARCH_RP2040
  EEPROM.begin(512);
#endif
}

void loadSettings() {
  EEPROM.get(SETTINGS_START, settings);
  Serial.println(F("Loading settings..."));
  if (settings.version != SETTINGS_VERSION) { // check if EEPROM version matches this code's version. re-initialize EEPROM if not matching
    // setup defaults
    settings.version = SETTINGS_VERSION;
    memset(settings.enabledShows, true, sizeof(settings.enabledShows)); // set all entries of enabledShows to true by default
    settings.navlights = true;
    Serial.println(F("New settings version. Setting defaults..."));
    
    saveSettings();
  }
}

void saveSettings() { // 
  EEPROM.put(SETTINGS_START, settings);
#ifdef ARDUINO_ARCH_RP2040
  EEPROM.commit();
#endif
  Serial.println(F("Saving settings..."));
}

void loadLayout() {
#ifdef ARDUINO_ARCH_RP2040
  EEPROM.get(LAYOUT_START, layout);
#else
  layout.version = pgm_read_word_near(LAYOUT_START);
#endif
  Serial.println(F("Loading layout..."));
  if (layout.version != LAYOUT_VERSION) { // check if EEPROM version matches this code's version. re-initialize EEPROM if not matching
    // setup defaults
    layout.version = LAYOUT_VERSION;
    layout.wing = 31;
    layout.nose = 4;
    layout.fuse = 18;
    layout.tail = 8;
    layout.wingnav = 8;
    layout.wingRev = false;
    layout.noseRev = false;
    layout.fuseRev = false;
    layout.tailRev = false;
    layout.nosefusejoined = true;
    Serial.println(F("New layout version. Setting defaults..."));
#ifdef ARDUINO_ARCH_RP2040
    EEPROM.put(LAYOUT_START, layout);
    EEPROM.commit();
#else
  } else {
    layout.wing           = pgm_read_byte_near(LAYOUT_START + 2);
    layout.nose           = pgm_read_byte_near(LAYOUT_START + 3);
    layout.fuse           = pgm_read_byte_near(LAYOUT_START + 4);
    layout.tail           = pgm_read_byte_near(LAYOUT_START + 5);
    layout.wingnav        = pgm_read_byte_near(LAYOUT_START + 6);
    layout.wingRev        = pgm_read_byte_near(LAYOUT_START + 7);
    layout.noseRev        = pgm_read_byte_near(LAYOUT_START + 8);
    layout.fuseRev        = pgm_read_byte_near(LAYOUT_START + 9);
    layout.tailRev        = pgm_read_byte_near(LAYOUT_START + 10);
    layout.nosefusejoined = pgm_read_byte_near(LAYOUT_START + 11);
#endif
  }
}

void printSettings() {
  Serial.println(F("----- Settings -----"));

#ifdef ARDUINO_ARCH_RP2040
  Serial.printf("Version:  %10X\r\n", settings.version);
#else
  Serial.print(F("Version: ")); Serial.println((unsigned int)settings.version, 16);
#endif

  for (int i = 0; i < numShows; i++) {
#ifdef ARDUINO_ARCH_RP2040
    Serial.printf("Show %3d:  %9s\r\n", i, settings.enabledShows[i] ? "enabled" : "disabled");
#else
    Serial.print(F("Show "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.println(enabled[settings.enabledShows[i]]);
#endif
  }

#ifdef ARDUINO_ARCH_RP2040
  Serial.printf("Navlights: %9s\r\n", settings.navlights ? "enabled" : "disabled");
#else
  Serial.print(F("Navlights: ")); Serial.println(enabled[settings.navlights]);
#endif
  Serial.println();
}

void printLayout() {
#ifdef ARDUINO_ARCH_RP2040
  Serial.println("------ Layout ------");
  Serial.printf("Version:    %8X\r\n", layout.version);
  Serial.printf("Wing:       %8d\r\n", layout.wing);
  Serial.printf("Nose:       %8d\r\n", layout.nose);
  Serial.printf("Fuse:       %8d\r\n", layout.fuse);
  Serial.printf("Tail:       %8d\r\n", layout.tail);
  Serial.printf("Navlights:  %8d\r\n", layout.wingnav);
  Serial.printf("Wing Rev:   %8s\r\n", layout.wingRev ? "true" : "false");
  Serial.printf("Nose Rev:   %8s\r\n", layout.noseRev ? "true" : "false");
  Serial.printf("Fuse Rev:   %8s\r\n", layout.fuseRev ? "true" : "false");
  Serial.printf("Tail Rev:   %8s\r\n", layout.tailRev ? "true" : "false");
  Serial.printf("N/F joined: %8s\r\n", layout.nosefusejoined ? "true" : "false");
  Serial.println();
#else
  Serial.println(F("------ Layout ------"));
  Serial.print(F("Version: ")); Serial.println((unsigned int)layout.version, 16);
  Serial.print(F("Wing: ")); Serial.println(layout.wing);
  Serial.print(F("Nose: ")); Serial.println(layout.nose);
  Serial.print(F("Fuse: ")); Serial.println(layout.fuse);
  Serial.print(F("Tail: ")); Serial.println(layout.tail);
  Serial.print(F("Navlights: ")); Serial.println(layout.wingnav);
  Serial.print(F("Wing Rev: ")); Serial.println(tf[layout.wingRev]);
  Serial.print(F("Nose Rev: ")); Serial.println(tf[layout.noseRev]);
  Serial.print(F("Fuse Rev: ")); Serial.println(tf[layout.fuseRev]);
  Serial.print(F("Tail Rev: ")); Serial.println(tf[layout.tailRev]);
  Serial.print(F("N/F joined:")); Serial.println(tf[layout.nosefusejoined]);
  Serial.println();
#endif
}