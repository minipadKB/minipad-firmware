#pragma once
#pragma GCC diagnostic ignored "-Wtype-limits"

#include "config/configuration.hpp"
#include "definitions.hpp"

inline class ConfigurationController
{
public:
    ConfigurationController()
    {
        defaultConfig = getDefaultConfig();
        config = defaultConfig;
    }

    void loadConfig();
    void saveConfig();

    Configuration config;

private:
    Configuration defaultConfig;

    // Default configuration loaded into the EEPROM if no configuration was saved yet. Also used to reset the keypad and calibration
    // structs that might get modified on a firmware update and have to be reset back to their default values then later on.
    Configuration getDefaultConfig()
    {
        Configuration config = Configuration();

        // Populate the hall effect keys array with the correct amount of hall effect keys.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            config.heKeys[i] = HEKey(i);

        // Populate the digital keys array with the correct amount of digital keys.
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            config.digitalKeys[i] = DigitalKey(i);

        return config;
    };

} ConfigController;
