#pragma once

#include <cstdint>
#include "config/keys/key.hpp"

// Configuration for the digital keys of the keypad.
struct DigitalKey : Key
{
    // Initialize with the correct type for identifying the type of key that a Key object was initialized as (e.g. DigitalKey) and index.
    // Assign the key char from a forwards (a, b, c, d, e, ...). After 26 keys, stick to an 'z' key to not overflow.
    DigitalKey(uint8_t index) : Key(KeyType::Digital, index, index >= 26 ? 'z' : (char)('a' + index)) {}

    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between Key and DigitalKey.
};
