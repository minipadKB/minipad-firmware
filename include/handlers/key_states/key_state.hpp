#pragma once

#include <Arduino.h>
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// The base struct containing info about the state of a key for the keypad handler.
struct KeyState
{
    // The index of the key. This is hardcoded in the default config and is not changed.
    uint8_t index;

    // State whether the key is currently pressed down.
    bool pressed = false;
};
