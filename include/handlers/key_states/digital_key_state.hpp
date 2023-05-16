#pragma once

#include <Arduino.h>
#include "handlers/key_states/key_state.hpp"
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct containing info about the state of a digital key for the keypad handler.
struct DigitalKeyState : KeyState
{
    // The last time a key press on the digital key was sent, in milliseconds since firmware bootup.
    unsigned long lastDebounce = 0;
};
