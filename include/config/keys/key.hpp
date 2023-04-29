#pragma once

#include <stdint.h>
#include "config/keys/key_type.hpp"

// The base configuration struct for the DigitalKey and HEKey struct, containing the common fields.
struct Key
{
    // The type of key this Key object represents.
    // Used to identify the type of key that a Key object was initialized as (e.g. HEKey).
    KeyType type = KeyType::Base;

    // The index of the key. This is hardcoded in the default config and is not changed.
    // It does not serve a config purpose but is instead for accessing the index from the DigitalKey object.
    uint8_t index;

    // The corresponding key sent via HID interface.
    char keyChar;

    // Bools whether HID commands are sent on the key.
    bool hidEnabled;
};
