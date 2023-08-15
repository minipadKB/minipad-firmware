#pragma once

#include <cstdint>

// The base configuration struct for the DigitalKeyConfig and HEKeyConfig struct, containing the common fields.
struct KeyConfig
{
    // Require every key config to be initialized with a key char.
    KeyConfig(char keyChar) : keyChar(keyChar) {}

    // The corresponding key sent via HID interface.
    char keyChar;

    // Bools whether HID commands are sent on the key.
    bool hidEnabled = false;
};
