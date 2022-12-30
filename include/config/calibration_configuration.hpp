#pragma once

#include <stdint.h>

#define CALIBRATION_CONFIGURATION_VERSION 0

struct CalibrationConfiguration
{
  // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
  uint16_t version;

  // The value read when the keys are in rest position
  uint16_t key1RestPosition;
  uint16_t key2RestPosition;

  // The value read when the keys are pressed all the way down
  uint16_t key1DownPosition;
  uint16_t key2DownPosition;
};