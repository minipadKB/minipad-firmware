#pragma once

#include <stdint.h>

struct CalibrationConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint32_t version;

  // The value read when the keys are in rest position
  uint16_t key1RestPosition;
  uint16_t key2RestPosition;

  // The value read when the keys are pressed all the way down
  uint16_t key1DownPosition;
  uint16_t key2DownPosition;

  static uint32_t getVersion()
  {
    // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:40am on the 3rd january 2023)
    return 2301030040;
  }
};