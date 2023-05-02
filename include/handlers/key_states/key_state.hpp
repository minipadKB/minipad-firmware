#pragma once

#include <Arduino.h>
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// The base struct containing info about the state of a key for the keypad handler.
struct KeyState
{
    // State whether the key is currently pressed down.
    bool pressed = false;
};
