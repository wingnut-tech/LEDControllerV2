#pragma once

#ifdef ARDUINO_ARCH_RP2040

#  define STATUS_LED 8
#  define PROGRAM_CYCLE_BTN 16
#  define PROGRAM_ENABLE_BTN 17
#  define TAIL_PIN 2
#  define FUSE_PIN 3
#  define NOSE_PIN 4
#  define LEFT_PIN 5
#  define RIGHT_PIN 6
#  define RC_PIN1 1
#  define RC_PIN2 0
#  define SDA_PIN 12
#  define SCL_PIN 13

#else

#  define STATUS_LED LED_BUILTIN
#  define PROGRAM_CYCLE_BTN 6
#  define PROGRAM_ENABLE_BTN 7
#  define TAIL_PIN 8
#  define FUSE_PIN 9
#  define NOSE_PIN 10
#  define LEFT_PIN 11
#  define RIGHT_PIN 12
#  define RC_PIN1 5
#  define RC_PIN2 4

#endif