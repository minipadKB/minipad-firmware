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
            .digitalKeys = {}
        };

        // Populate the hall effect keys array with the correct amount of hall effect keys.
        for (uint8_t i = 0; i < HE_KEYS; i++)
        {
            config.heKeys[i] = HEKey();
            config.heKeys[i].index = i;

            // Assign the keys from z downwards. (z, y, x, w, v, ...)
            // After 26 keys, stick to an 'a' key to not overflow.
            config.heKeys[i].keyChar = i >= 26 ? 'a' : (char)('z' - i);

            config.heKeys[i].hidEnabled = false;
            config.heKeys[i].rapidTrigger = false;
            config.heKeys[i].continuousRapidTrigger = false;

            // For the default values of these values, a value depending on the total travel distance is being used.
            config.heKeys[i].rapidTriggerUpSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;
            config.heKeys[i].rapidTriggerDownSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;
            config.heKeys[i].lowerHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.55);
            config.heKeys[i].upperHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.675);

            // Set the calibration values to the outer boundaries of the possible values to "disable" them.
            config.heKeys[i].restPosition = pow(2, ANALOG_RESOLUTION) - 1;
            config.heKeys[i].downPosition = 0;
        }

        // Populate the digital keys array with the correct amount of digital keys.
#pragma GCC diagnostic ignored "-Wtype-limits"
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
#pragma GCC diagnostic pop
        {
            config.digitalKeys[i] = DigitalKey();
            config.digitalKeys[i].index = i;

            // Assign the keys from a forwards. (a, b, c, d, e, ...)
            // After 26 keys, stick to an 'z' key to not overflow.
            config.digitalKeys[i].keyChar = i >= 26 ? 'z' : (char)('a' + i),
            config.digitalKeys[i].hidEnabled = false;
        }

#pragma GCC diagnostic ignored "-Wtype-limits"
        for (uint8_t i = 0; i < LEDS; i++)
#pragma GCC diagnostic pop
        {
            config.leds[i] = Led();

            // Set the default ARGB for the leds to half-brightness (80) and white (FFFFFF).
            config.leds[i].argb = 0x80FFFFFF;
        }

        return config;
    };

} ConfigController;
