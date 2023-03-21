#include <EEPROM.h>
#include <Arduino.h>
#include "config/configuration_controller.hpp"

void ConfigurationController::loadConfig()
{
    // Load the configuration struct from the EEPROM.
    EEPROM.get(0, config);

    // Check if all versions match with the ones read; If not, replace the (part of the) config with it's default state.
    bool save = false;
    if (config.version != defaultConfig.version)
    {
        config = defaultConfig;
        save = true;
    }
    if (config.keys[0].version != defaultConfig.keys[0].version)
    {
        for (int i = 0; i < KEYS; i++)
            config.keys[i] = defaultConfig.keys[i];
        save = true;
    }

    // If anything has been modified, save the config to the EEPROM.
    if (save)
        saveConfig();
}

void ConfigurationController::saveConfig()
{
    // Write the struct back into the EEPROM and commit the change.
    EEPROM.put(0, config);
    EEPROM.commit();
}