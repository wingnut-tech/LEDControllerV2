#pragma once

#include "shows.h"

#define SETTINGS_VERSION 0xAA03 // EEPROM config version (increment this any time the Config struct or number of shows changes).
#define SETTINGS_START 0 // starting EEPROM address for our config

#define LAYOUT_VERSION 0xAA03 // EEPROM config version (increment this any time the Layout struct changes).

#ifdef ARDUINO_ARCH_RP2040
#  define LAYOUT_START 256 // starting EEPROM address for our layout
#else
#  define LAYOUT_START (0x7E00-16)
#endif

struct Settings {
  uint16_t version; // what version this settings struct is
  bool navlights; // whether navlights are enabled/disabled
  bool enabledShows[NUM_SHOWS_WITH_ALTITUDE]; // which shows are active/disabled
};

struct Layout {
  uint16_t version; // what version this layout struct is
  uint8_t wing;
  uint8_t nose;
  uint8_t fuse;
  uint8_t tail;
  uint8_t wingnav;
  bool wingRev;
  bool noseRev;
  bool fuseRev;
  bool tailRev;
  bool nosefusejoined;
};

extern Settings settings;
extern Layout layout;

void initEEPROM();

void loadSettings();

void saveSettings();

void loadLayout();

void printLayout();

void printSettings();