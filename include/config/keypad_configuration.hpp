#pragma once

#include <stdint.h>

#define KEYPAD_CONFIGURATION_VERSION 0

struct KeypadConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint16_t version;

  // Bool whether rapid trigger is enabled or not
  bool rapidTrigger;

  // The sensitivity of the rapid trigger
  uint16_t rapidTriggerSensitivity;

  // The value below which the button is pressed if not using rapid trigger
  uint16_t lowerHysteresis;

  // The value above which the button is released if not using rapid trigger
  uint16_t upperHysteresis;

  // The corresponding keys sent via HID interface
  char key1;
  char key2;

  // Bools whether HID commands are sent on that key
  bool key1HIDEnabled;
  bool key2HIDEnabled;
};