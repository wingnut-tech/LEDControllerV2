#include "../LEDController.h"
#include "eeprom.h"
#include "LED.h"

#include <EEPROM.h>

extern Settings settings;

extern uint8_t numShows;
extern uint8_t numActiveShows;
extern uint8_t activeShowNumbers[];

extern LED Right;
extern LED Left;

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
  } else { // only run update if we didn't just make defaults, as saveConfig() already does this
    updateShowSettings();
  }
}

void saveSettings() { // 
  EEPROM.put(SETTINGS_START, settings);
  Serial.println(F("Saving settings..."));
  updateShowSettings();
}

void updateShowSettings() {
  uint8_t active = 0;

  Serial.print(F("Settings version: "));
  Serial.println(settings.version);

  for (int i = 0; i < numShows; i++) {
    Serial.print(F("Show "));
    Serial.print(i);
    Serial.print(F(": "));
    if (settings.enabledShows[i]) {
      Serial.println(F("enabled."));
      activeShowNumbers[active] = i;
      active++;
    } else {
      Serial.println(F("disabled."));
    }
  }
  numActiveShows = active;

  Serial.print(F("Navlights: "));
  if (settings.navlights) { // navlights are on, set stopPoint to (total number) - (number of navlights)
    Right.stopPoint = WING_LEDS - WING_NAV_LEDS;
    Left.stopPoint = WING_LEDS - WING_NAV_LEDS;
    Serial.println(F("on."));
  } else { // navlights are off, set stopPoint to max number of LEDs
    Right.stopPoint = WING_LEDS;
    Left.stopPoint = WING_LEDS;
    Serial.println(F("off."));
  }
}
