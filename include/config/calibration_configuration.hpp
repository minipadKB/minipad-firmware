#pragma once

#include <stdint.h>

// Configuration for the calibration of the keypad, containing the rest and down positions of the keys.
struct CalibrationConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint32_t version;

  // The value read when the keys are in rest position/all the way down.
  uint16_t key1RestPosition;
  uint16_t key1DownPosition;
  uint16_t key2RestPosition;
  uint16_t key2DownPosition;
#ifdef KEYS_3
  uint16_t key3RestPosition;
  uint16_t key3DownPosition;
#endif

  // Returns the version constant of the latest CalibrationConfiguration layout.
  static uint32_t getVersion()
  {
    // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
    int64_t version = 2302031804;

    // To reset the configuration if the user switches from a 2-key firmware to a 3-key, mutate the version.
#ifdef KEYS_3
    return -version;
#else
    return version;
#endif
  }
};