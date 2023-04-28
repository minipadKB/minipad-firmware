#pragma once

#include <Arduino.h>
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct containing info about the state of a digital key for the keypad handler.
struct DigitalKeyState
{
    // State whether the digital key is currently pressed down.
    bool pressed = false;
};
