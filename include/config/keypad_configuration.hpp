#pragma once

#include <stdint.h>
#include "definitions.hpp"

// Configuration for the keypad itself, containing the rapid trigger, hysteresis and key settings.
struct KeypadConfiguration
{
    // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
    int64_t version;

    // Bool whether rapid trigger is enabled or not.
    bool rapidTrigger;

    // Bool whether continuous rapid trigger is enabled or not.
    // This ignores the hysteresis once it has been passed until it is fully released.
    bool continuousRapidTrigger;

    // The sensitivity of the rapid trigger algorithm when pressing up.
    uint16_t rapidTriggerUpSensitivity;

    // The sensitivity of the rapid trigger algorithm when pressing down.
    uint16_t rapidTriggerDownSensitivity;

    // The value below which the button is pressed in traditional mode or rapid trigger is active in rapid trigger mode.
    uint16_t lowerHysteresis;

    // The value below which the button is pressed in traditional mode or rapid trigger is not active in rapid trigger mode.
    uint16_t upperHysteresis;

    // The corresponding keys sent via HID interface
    // Bools whether HID commands are sent on the keys.
#ifdef KEYS_3
    char keyChars[3];
#else
    char keyChars[2];
#endif

    // Bools whether HID commands are sent on the keys.
#ifdef KEYS_3
    bool hidEnabled[3];
#else
    bool hidEnabled[2];
#endif

    // Returns the version constant of the latest KeypadConfiguration layout.
    static int64_t getVersion()
    {
        // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
        int64_t version = 2302061505;

        // To reset the configuration if the user switches from a 2-key firmware to a 3-key, mutate the version.
#ifdef KEYS_3
        return -version;
#else
        return version;
#endif
    }
};