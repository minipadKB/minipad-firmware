#pragma once

#include "keypad_configuration.hpp"
#include "calibration_configuration.hpp"
#include "tolerance_configuration.hpp"

// Configuration for the whole firmware, containing the name of the keypad and it's configurations.
struct Configuration
{
    // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
    uint32_t version;

    // The name of the keypad, used to distinguish it from others.
    char name[128];

    // The configuration of the keypad itself. (rapid trigger, hysteresis, ...)
    KeypadConfiguration keypad;

    // The calibration values of the keypad. (rest and down positions)
    CalibrationConfiguration calibration;

    // Returns the version constant of the latest Configuration layout.
    static uint32_t getVersion()
    {
        // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:40am on the 3rd january 2023)
        return 2301030040;
    }
};