#pragma once

#include <stdint.h>

// An enum used to identify the type of key a Key object represents.
enum KeyType
{
    // Key objects of this type have been initialized as neither an HEKey or DigitalKey object.
    Base,

    // A Key object that was initialized as an HEKey object.
    HallEffect,

    // A Key object that was initialized as a DigitalKey object.
    Digital
};


// The base configuration struct for the DigitalKey and HEKey struct, containing the common fields.
struct Key
{
    // Used to identify the type of key that a Key object was initialized as (e.g. HEKey or DigitalKey).
    KeyType type = KeyType::Base;

    // The index of the key. This is hardcoded in the default config and is not changed.
    // It does not serve a config purpose but is instead for accessing the index from the DigitalKey object.
    uint8_t index;

    // The corresponding key sent via HID interface.
    char keyChar;

    // Bools whether HID commands are sent on the key.
    bool hidEnabled;
};

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

// Configuration for the digital keys of the keypad.
struct DigitalKey : Key
{
    // Initialize with the correct type for identifying the type of key that a Key object was initialized as (e.g. DigitalKey).
    DigitalKey() { type = KeyType::Digital; }

    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between Key and DigitalKey.
};
