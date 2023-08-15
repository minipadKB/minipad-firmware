#pragma once

#include <cstdint>
#include "config/keys/key_config.hpp"
#include "definitions.hpp"

// Configuration for the Hall Effect keys of the keypad, containing the actuation points, calibration, sensitivities etc. of the key.
struct HEKeyConfig : KeyConfig
{
    // Default constructor for the HEKeyConfig struct for initializing the arrays in the Configuration struct.
    HEKeyConfig() : KeyConfig('\0') {}

    // Initialize with the specified key char.
    HEKeyConfig(char keyChar) : KeyConfig(keyChar) {}

    // Bool whether rapid trigger is enabled or not.
    bool rapidTrigger = false;

    // Bool whether continuous rapid trigger is enabled or not.
    bool continuousRapidTrigger = false;

    // The sensitivity of the rapid trigger algorithm when pressing up.
    uint16_t rapidTriggerUpSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;

    // The sensitivity of the rapid trigger algorithm when pressing down.
    uint16_t rapidTriggerDownSensitivity = TRAVEL_DISTANCE_IN_0_01MM / 10;

    // The value below which the key is pressed and rapid trigger is active in rapid trigger mode.
    uint16_t lowerHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.55);

    // The value below which the key is no longer pressed and rapid trigger is no longer active in rapid trigger mode.
    uint16_t upperHysteresis = (uint16_t)(TRAVEL_DISTANCE_IN_0_01MM * 0.675);
};
