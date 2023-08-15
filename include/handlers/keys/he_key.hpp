#pragma once

#include <Arduino.h>
#include "config/keys/he_key_config.hpp"
#include "handlers/keys/key.hpp"
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct representing a Hall Effect key, including it's current runtime state and HEKeyConfig object.
struct HEKey : Key
{
    // Default constructor for the HEKey struct for initializing the arrays in the KeyHandler class.
    HEKey() : Key(0, nullptr) {}

    // Require every HEKey object to pass a KeyConfig object to the underlaying Key object.
    HEKey(uint8_t index, HEKeyConfig *config) : Key(index, config), config(config) {}

    // The HEKeyConfig object of this Hall Effect key.
    HEKeyConfig *config;

    // State whether the hall effect key is currently inside the rapid trigger zone (below the lower hysteresis).
    bool inRapidTriggerZone = false;

    // The current peak value for the rapid trigger logic.
    uint16_t rapidTriggerPeak = UINT16_MAX;

    // The raw value with low-pass filter applied read from the Hall Effect sensor.
    uint16_t rawValue = 0;

    // The distance of the magnet from the sensor, calculated through the raw value.
    uint16_t distance = 0;

    // The highest and lowest values ever read on the sensor. Used for calibration purposes,
    // specifically mapping future values read from the sensors from this range to 0.01mm steps.
    // By default, set the range from (1<<analog_resolution)-1 to 0 so it can be updated.
    uint16_t restPosition = 0;
    uint16_t downPosition = (1 << ANALOG_RESOLUTION) - 1;

    // The simple moving average filter for stabilizing the analog outpt.
    SMAFilter filter = SMAFilter(SMA_FILTER_SAMPLE_EXPONENT);
};
