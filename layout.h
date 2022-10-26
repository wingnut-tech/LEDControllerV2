#pragma once

// Layout: FT Night Radian

// number of LEDs in specific strings
#define WING_LEDS 31 // total wing LEDs
#define NOSE_LEDS 4 // total nose LEDs
#define FUSE_LEDS 18 // total fuselage LEDs
#define TAIL_LEDS 8 // total tail LEDs

// strings reversed?
#define WING_REV false
#define NOSE_REV false
#define FUSE_REV false
#define TAIL_REV false

#define NOSE_FUSE_JOINED true // are the nose and fuse strings joined?
#define WING_NAV_LEDS 8 // wing LEDs that are navlights

// mA per LED. for the Radian, 18 makes it run at full brightness
#define LED_POWER 18

// MAX_POWER and LED_POWER are used together to calculate the max brightness.
// uncomment if you need a lower limit (eg. a smaller battery):
//#define MAX_POWER 1800