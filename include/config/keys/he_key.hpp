#pragma once

#include <stdint.h>
#include "config/keys/key.hpp"
#include "config/keys/key_type.hpp"

// Configuration for the hall effect keys of the keypad, containing the actuation points, calibration, sensitivities etc. of the key.
struct HEKey : Key
{
    // Initialize with the correct type for identifying the type of key that a Key object was initialized as (e.g. HEKey).
    HEKey() { type = KeyType::HallEffect; }

    // Bool whether rapid trigger is enabled or not.
    bool rapidTrigger;

    // Bool whether continuous rapid trigger is enabled or not.
    bool continuousRapidTrigger;

    // The sensitivity of the rapid trigger algorithm when pressing up.
    uint16_t rapidTriggerUpSensitivity;

    // The sensitivity of the rapid trigger algorithm when pressing down.
    uint16_t rapidTriggerDownSensitivity;

    // The value below which the key is pressed and rapid trigger is active in rapid trigger mode.
    uint16_t lowerHysteresis;

    // The value below which the key is no longer pressed and rapid trigger is no longer active in rapid trigger mode.
    uint16_t upperHysteresis;

    // The value read when the keys are in rest position/all the way down.
    uint16_t restPosition;
    uint16_t downPosition;
};
