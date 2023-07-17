#include "input.h"

#include "pins.h"
#include "mode.h"

uint16_t currentCh1 = 0;
uint16_t currentCh2 = 0;
uint8_t currentCh = EITHER;

#ifdef ARDUINO_ARCH_RP2040
// the rp2040 can wait a whole 30ms for a pulse, since it's happening on the second core
#  define PULSE_WAIT 30000
#else
// the atmega seems to work fine with shorter 5ms pulseIn calls, and only grabbing one every so often
#  define PULSE_WAIT 5000
#endif

void readInputs() {
  static uint8_t misses = 0;

  uint16_t pwm;
  bool read = false;
  if (currentCh == CH1 || currentCh == EITHER) {
    pwm = pulseIn(RC_PIN1, HIGH, PULSE_WAIT);
    if (pwm > 700 && pwm < 2400) {
      currentCh = CH1;
      currentCh1 = pwm;
      read = true;
    }
  }
  if (currentCh == CH2 || currentCh == EITHER) {
    pwm = pulseIn(RC_PIN2, HIGH, PULSE_WAIT);
    if (pwm > 700 && pwm < 2400) {
      currentCh = CH2;
      currentCh2 = pwm;
      read = true;
    }
  }

  if (read) {
    misses = 0;
    haveSignal = true;
  } else {
    misses++;
  }

  if (misses >= 100) {
    currentCh = EITHER;
    haveSignal = false;
    misses = 0;
  }
}