#pragma once

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
        Configuration config = {
            .name = {'m', 'i', 'n', 'i', 'p', 'a', 'd'},
            .heKeys = {},
            .digitalKeys = {}};

        // Populate the hall effect keys array with the correct amount of hall effect keys.
        for (uint8_t i = 0; i < HE_KEYS; i++)
        {
            config.heKeys[i] = HEKey();
            config.heKeys[i].index = i;

            // Assign the keys from z downwards. (z, y, x, w, v, ...)
            // With too many keys (>26?), this would eventually run out of characters.
            config.heKeys[i].keyChar = (char)('z' - i);

            config.heKeys[i].hidEnabled = false;
            config.heKeys[i].rapidTrigger = false;
            config.heKeys[i].continuousRapidTrigger = false;

            // For the default values of these values, a value depending on the total travel distance is being used.
            config.heKeys[i].rapidTriggerUpSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;
            config.heKeys[i].rapidTriggerDownSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;
            config.heKeys[i].lowerHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.55);
            config.heKeys[i].upperHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.675);

            // Approximate calibration values on a 49E sensor with 12-bit resolution.
            config.heKeys[i].restPosition = 1800;
            config.heKeys[i].downPosition = 1100;
        }

        // Populate the digital keys array with the correct amount of digital keys.
#pragma GCC diagnostic ignored "-Wtype-limits"
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
#pragma GCC diagnostic pop
        {
            config.digitalKeys[i] = DigitalKey();
            config.digitalKeys[i].index = i;

            // Assign the keys from a forwards. (z, y, x, w, v, ...)
            // With too many keys (>26?), this would eventually run out of characters.
            config.digitalKeys[i].keyChar = (char)('a' + i),
            config.digitalKeys[i].hidEnabled = false;
        }

        return config;
    };

} ConfigController;
