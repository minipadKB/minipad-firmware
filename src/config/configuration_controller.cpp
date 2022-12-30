#include <EEPROM.h>
#include <Arduino.h>
#include "config/configuration_controller.hpp"

ConfigurationController::ConfigurationController(Configuration *defaultConfig) : defaultConfig(defaultConfig), config(*defaultConfig) {}
ConfigurationController::~ConfigurationController() {}

void ConfigurationController::loadConfig()
{
  // Load the configuration struct from the EEPROM
  EEPROM.get(0, config);

  // Check if all versions match with the ones read; If not, replace the (part of the) config with it's default state
  bool save = false;
  if (config.version != defaultConfig->version)
  {
    config = *defaultConfig;
    save = true;
  }

  if (config.keypad.version != defaultConfig->keypad.version)
  {
    config.keypad = defaultConfig->keypad;
    save = true;
  }
  if (config.calibration.version != defaultConfig->calibration.version)
  {
    config.calibration = defaultConfig->calibration;
    save = true;
  }

  // If anything has been modified, save the config to the EEPROM
  if (save)
    saveConfig();
}

void ConfigurationController::saveConfig()
{
  // Write the struct back into the EEPROM
  EEPROM.put(0, config);
}