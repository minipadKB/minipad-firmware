#pragma once

#include <stdint.h>

// Configuration for the digital keys of the keypad, containing the key char and hid state of the key.
struct DigitalKey
{
    // Version of the configuration, used to check whether the struct layout in the EEPROM is up-to-date.
    int64_t version = DigitalKey::getVersion();

    // The index of the key. This is hardcoded in the default config and is not changed.
    // It does not serve a config purpose but is instead for accessing the index from the DigitalKey object.
    uint8_t index;

    // The corresponding key sent via HID interface.
    char keyChar;

    // Bools whether HID commands are sent on the key.
    bool hidEnabled;

    // Returns the version constant of the latest configuration layout.
    static uint32_t getVersion()
    {
        // Version of the configuration in the format YYMMDDhhmm (e.g. 2301030040 for 12:44am on the 3rd january 2023)
        return 2304281039;
    }
};
