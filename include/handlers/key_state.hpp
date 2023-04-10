#pragma once

#include <Arduino.h>
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct containing info about the state of a key for the keypad handler.
struct KeyState
{
    // State whether the key is currently pressed down.
    bool pressed = false;

    // State whether the key is currently inside the rapid trigger zone (below the lower hysteresis).
    bool inRapidTriggerZone = false;

    // The current peak value for the rapid trigger logic.
    uint16_t rapidTriggerPeak = 65535;

    // The simple moving average filter for stabilizing the analog outpt.
    SMAFilter filter = SMAFilter(SMA_FILTER_SAMPLE_EXPONENT);
};
