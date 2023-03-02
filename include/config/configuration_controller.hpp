#pragma once

#include "config/configuration.hpp"

// Default configuration file loaded into the EEPROM if no configuration was saved yet. Also used to reset the keypad and calibration
// structs that might get modified on a firmware update and have to be reset back to their default values then later on.
inline Configuration defaultConfig =
{
  .version = Configuration::getVersion(),
  .name = {'m', 'i', 'n', 'i', 'p', 'a', 'd' },
  .keypad =
  {
    .version = KeypadConfiguration::getVersion(),
    .rapidTrigger = false,
    .continuousRapidTrigger = false,
    .rapidTriggerUpSensitivity = 50,
    .rapidTriggerDownSensitivity = 50,
    .lowerHysteresis = 250,
    .upperHysteresis = 300,
#if KEYS == 3
    .keyChars = { 'z', 'x', 'c' },
    .hidEnabled = { true, true, true }
#else
    .keyChars = { 'z', 'x' },
    .hidEnabled = { true, true }
#endif
  },
  .calibration =
  {
    .version = CalibrationConfiguration::getVersion(),
#if KEYS == 3
    .restPositions = { 1800, 1800, 1800 },
    .downPositions = { 1100, 1100, 1100 }
#else
    .restPositions = { 1800, 1800 },
    .downPositions = { 1100, 1100 }
#endif
  }
};

inline class ConfigurationController
{
public:
    ConfigurationController(Configuration *defaultConfig);

    void loadConfig();
    void saveConfig();

    Configuration config;
} ConfigController(&defaultConfig);