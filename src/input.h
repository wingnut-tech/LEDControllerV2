#pragma once

#include <Arduino.h>

enum { EITHER, CH1, CH2, };

extern uint16_t currentCh1;
extern uint16_t currentCh2;
extern uint8_t currentCh;


void readInputs();