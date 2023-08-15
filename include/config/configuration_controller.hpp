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
        Configuration config;

        // Populate the Hall Effect keys array with the correct amount of Hall Effect keys.
        // Assign the key char from z downwards (z, y, x, w, v, ...). After 26 keys, stick to an 'a' key to not overflow.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            config.heKeys[i] = HEKeyConfig(i >= 26 ? 'a' : (char)('z' - i));

        // Populate the digital keys array with the correct amount of digital keys.
    // Assign the key char from a forwards (a, b, c, d, e, ...). After 26 keys, stick to an 'z' key to not overflow.
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            config.digitalKeys[i] = DigitalKeyConfig(i >= 26 ? 'z' : (char)('a' + i));

        return config;
    };

} ConfigController;
