#pragma once

#include "config/keys/he_key.hpp"
#include "config/keys/digital_key.hpp"

// Configuration for the whole firmware, containing the name of the keypad and it's configurations.
struct Configuration
{
    // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
    uint32_t version = Configuration::getVersion();

    // The name of the keypad, used to distinguish it from others.
    char name[128];

    // A list of all hall effect key configurations. (rapid trigger, hysteresis, calibration, ...)
    HEKey heKeys[HE_KEYS];

    // A list of all digital key configurations. (key char, hid state, ...)
    DigitalKey digitalKeys[DIGITAL_KEYS];

    // Returns the version constant of the latest Configuration layout.
    static uint32_t getVersion()
    {
        // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
        int64_t version = 2305261535;

        return version;
    }
};
