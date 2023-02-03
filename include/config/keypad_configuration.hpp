#pragma once

#include <stdint.h>
#include "definitions.hpp"

// Configuration for the keypad itself, containing the rapid trigger, hysteresis and key settings.
struct KeypadConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint32_t version;

  // Bool whether rapid trigger is enabled or not
  bool rapidTrigger;

  // Bool whether continuous rapid trigger is enabled or not
  bool continuousRapidTrigger;

  // The sensitivity of the rapid trigger
  uint16_t rapidTriggerSensitivity;

  // The value below which the button is pressed if not using rapid trigger
  uint16_t lowerHysteresis;

  // The value above which the button is released if not using rapid trigger
  uint16_t upperHysteresis;

  // The corresponding keys sent via HID interface
  char key1;
  char key2;
#ifdef KEYS_3
  char key3;
#endif

  // Bools whether HID commands are sent on that key
  bool key1HIDEnabled;
  bool key2HIDEnabled;
#ifdef KEYS_3
  bool key3HIDEnabled;
#endif

  // Returns the version constant of the latest KeypadConfiguration layout.
  static int64_t getVersion()
  {
    // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
    int64_t version = 2302031806;

    // To reset the configuration if the user switches from a 2-key firmware to a 3-key, mutate the version.
#ifdef KEYS_3
    return -version;
#else
    return version;
#endif
  }
};