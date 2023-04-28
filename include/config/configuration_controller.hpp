#pragma once

#include "config/configuration.hpp"
#include "definitions.hpp"

// Default configuration loaded into the EEPROM if no configuration was saved yet. Also used to reset the keypad and calibration
// structs that might get modified on a firmware update and have to be reset back to their default values then later on.
Configuration getDefaultConfig()
{
    Configuration config = {
        .name = {'m', 'i', 'n', 'i', 'p', 'a', 'd'},
        .heKeys = {}
    };

    // Populate the hall effect keys array with the correct amount of hall effect keys.
    for(uint8_t i = 0; i < HE_KEYS; i++)
    {
        config.heKeys[i] = {
            .index = i,
            .rapidTrigger = false,
            .continuousRapidTrigger = false,

            // For the default values of these values, a value depending on the total travel distance is being used.
            .rapidTriggerUpSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10,
            .rapidTriggerDownSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10,
            .lowerHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.55),
            .upperHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.675),

             // Assign the keys from z downwards. (z, y, x, w, v, ...)
             // With too many keys (>26?), this would eventually run out of characters.
            .keyChar = (char)('z' - i),
            .hidEnabled = false,
            .restPosition = 1800,
            .downPosition = 1100
        };
    }

    return config;
};

inline class ConfigurationController
{
public:
    ConfigurationController()
    {
        defaultConfig = getDefaultConfig();
        config = defaultConfig;
    };

    void loadConfig();
    void saveConfig();

    Configuration config;
    Configuration defaultConfig;
} ConfigController;
