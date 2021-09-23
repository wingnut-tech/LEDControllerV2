#pragma once

#include "../LEDController.h"

#define SETTINGS_VERSION 0xAA03 // EEPROM config version (increment this any time the Config struct or number of shows changes).
#define SETTINGS_START 0 // starting EEPROM address for our config

// Struct: Settings
// --------------
//   this is the main settings struct that holds everything we want to save/load from EEPROM
typedef struct Settings {
  int version; // what version this settings struct is
  bool navlights; // whether navlights are enabled/disabled
  bool enabledShows[NUM_SHOWS_WITH_ALTITUDE]; // which shows are active/disabled
};

// Function: loadSettings
// --------------------
//   loads existing settings from EEPROM, or if version mismatch, sets up new defaults and saves them
void loadSettings();

// Function: saveSettings
// --------------------
//   saves current settings to EEPROM
void saveSettings();

// Function: updateShowSettings
// --------------------------
//   sets order of currently active shows. e.g., activeShowNumbers[] = {1, 4, 5, 9}
//   also sets nav stop point
void updateShowSettings();