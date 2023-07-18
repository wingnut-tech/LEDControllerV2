
#include "bmx280.h"

#include "pins.h"

#include <Wire.h>
#include <ErriezBMX280.h>

ErriezBMX280 bmx280 = ErriezBMX280(BMX280_I2C_ADDR);

static float basePressure; // gets initialized with ground level pressure on startup
static bool hasBMX280 = false;


void initBMX() {
#ifdef ARDUINO_ARCH_RP2040
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
#endif
  Wire.begin();
  Wire.setClock(400000);

  if (bmx280.begin()) {
    hasBMX280 = true;
  }

  if (hasBMX280) {
    bmx280.setSampling(BMX280_MODE_NORMAL,      /* Operating Mode. */
                       BMX280_SAMPLING_X2,      /* Temp. oversampling */
                       BMX280_SAMPLING_X16,     /* Pressure oversampling */
                       BMX280_SAMPLING_X16,     /* Humidity oversampling */
                       BMX280_FILTER_X4,        /* Filtering. */
                       BMX280_STANDBY_MS_62_5); /* Standby time. */

    basePressure = bmx280.readPressure()/100; // this gets the current pressure at ground level, so we can get relative altitude
  }
}


void printBMX() {
  if (hasBMX280) {
    Serial.println(F("BMX280 module found."));
    Serial.print(F("Base pressure: "));
    Serial.print(basePressure);
    Serial.println(F("hPa"));
    Serial.println();
  } else {
    Serial.println(F("No BMX280 module found."));
    Serial.println();
  }
}


double getAltitude() {
  return bmx280.readAltitude(basePressure)*3.3; // *3.3 to convert meters to feet
}