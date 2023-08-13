#pragma once

#include <Arduino.h>
#include "handlers/key_states/key_state.hpp"
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct containing info about the state of a hall effect key for the keypad handler.
struct HEKeyState : KeyState
{
    // State whether the hall effect key is currently pressed down.
    bool pressed = false;

    // State whether the hall effect key is currently inside the rapid trigger zone (below the lower hysteresis).
    bool inRapidTriggerZone = false;

    // The current peak value for the rapid trigger logic.
    uint16_t rapidTriggerPeak = 65535;

    // The last value read from the hall effect sensor.
    uint16_t lastSensorValue = 0;

    // The mapped version of the value read from the hall effect sensor.
    uint16_t lastMappedValue = 0;

    // The simple moving average filter for stabilizing the analog outpt.
    SMAFilter filter = SMAFilter(SMA_FILTER_SAMPLE_EXPONENT);
};
