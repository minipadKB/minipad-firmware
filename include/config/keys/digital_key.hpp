#pragma once

#include <stdint.h>
#include "key.hpp"

// Configuration for the digital keys of the keypad.
struct DigitalKey : Key
{
    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between Key and HEKey.
};
