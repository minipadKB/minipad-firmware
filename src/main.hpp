
#include <Arduino.h>
#include "definitions.hpp"

uint16_t mapToRange400(uint16_t value, uint16_t min, uint16_t max);
void pressKey1();
void releaseKey1();
void pressKey2();
void releaseKey2();
#ifdef KEYS_3
void pressKey3();
void releaseKey3();
#endif