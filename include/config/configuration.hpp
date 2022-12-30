#pragma once

#include "keypad_configuration.hpp"
#include "calibration_configuration.hpp"

#define CONFIGURATION_VERSION 3256

struct Configuration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint16_t version;

  // The name of the keypad, used to distinguish it from other
  char name[128];

  // The configuration of the keypad (rapid trigger, hysteresis etc.)
  KeypadConfiguration keypad;

  // The calibration values of the keypad
  CalibrationConfiguration calibration;
};