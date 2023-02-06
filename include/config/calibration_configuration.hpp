#pragma once

#include <stdint.h>

// Configuration for the calibration of the keypad, containing the rest and down positions of the keys.
struct CalibrationConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  int64_t version;

  // The value read when the keys are in rest position/all the way down.
#ifdef KEYS_3
  uint16_t restPositions[3];
  uint16_t downPositions[3];
#else
  uint16_t restPositions[2];
  uint16_t downPositions[2];
#endif

  // Returns the version constant of the latest CalibrationConfiguration layout.
  static uint32_t getVersion()
  {
    // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
    int64_t version = 2302060002;

    // To reset the configuration if the user switches from a 2-key firmware to a 3-key, mutate the version.
#ifdef KEYS_3
    return -version;
#else
    return version;
#endif
  }
};