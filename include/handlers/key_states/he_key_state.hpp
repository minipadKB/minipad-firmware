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
    uint16_t rapidTriggerPeak = UINT16_MAX;

    // The last value read from the hall effect sensor.
    uint16_t lastSensorValue = 0;

    // The mapped version of the value read from the hall effect sensor.
    uint16_t lastMappedValue = 0;

    // The highest and lowest values ever read on the sensor. Used for calibration purposes,
    // specifically mapping future values read from the sensors from this range to 0.01mm steps.
    // By default, set the range from (1<<analog_resolution)-1 to 0 so it can be updated.
    uint16_t restPosition = 0;
    uint16_t downPosition = (1 << ANALOG_RESOLUTION) - 1;

    // The simple moving average filter for stabilizing the analog outpt.
    SMAFilter filter = SMAFilter(SMA_FILTER_SAMPLE_EXPONENT);
};
