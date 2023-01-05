#pragma once

#include <stdint.h>

struct ToleranceConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint32_t version;

  // Returns the version constant of the latest ToleranceConfiguration layout.
  static uint32_t getVersion()
  {
    // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:40am on the 3rd january 2023)
    return 2301052305;
  }
};