#pragma once

#include <Arduino.h>
#include "handlers/key_states/key_state.hpp"
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct containing info about the state of a digital key for the keypad handler.
struct DigitalKeyState : KeyState
{
    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between KeyState and HEKeyState.
};
