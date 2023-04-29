#include <EEPROM.h>
#include <Arduino.h>
#include "config/configuration_controller.hpp"

void ConfigurationController::loadConfig()
{
    // Load the configuration struct from the EEPROM.
    EEPROM.get(0, config);

    // Check if the version matches with the one read; If not, replace the config with it's default state.
    if (config.version != defaultConfig.version)
    {
        config = defaultConfig;
        saveConfig();
    }
}

void ConfigurationController::saveConfig()
{
    // Write the struct back into the EEPROM and commit the change.
    EEPROM.put(0, config);
    EEPROM.commit();
}
