#pragma once

#include <stdint.h>
#include "config/keys/key.hpp"

// Configuration for the digital keys of the keypad.
struct DigitalKey : Key
{
    // Hides the type field of the underlying Key object and replaces it with the HallEffect key type.
    // Used to identify the type of key that a Key object was initialized as (e.g. DigitalKey).
    KeyType type = KeyType::Digital;

    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between Key and DigitalKey.
};
