#pragma once

#include <cstdint>
#include "config/keys/key.hpp"

// Configuration for the digital keys of the keypad.
struct DigitalKey : Key
{
    // Initialize with the correct type for identifying the type of key that a Key object was initialized as (e.g. DigitalKey).
    DigitalKey() { type = KeyType::Digital; }

    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between Key and DigitalKey.
};
