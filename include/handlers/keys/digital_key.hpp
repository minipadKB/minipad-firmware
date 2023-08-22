#pragma once

#include <Arduino.h>
#include "config/keys/digital_key_config.hpp"
#include "handlers/keys/key.hpp"
#include "helpers/sma_filter.hpp"
#include "definitions.hpp"

// A struct representing a digital key, including it's current runtime state and DigitalKeyConfig object.
struct DigitalKey : Key
{
    // Default constructor for the DigitalKey struct for initializing the arrays in the KeyHandler class.
    DigitalKey() : Key(0, nullptr) {}

    // Require every DigitalKey object to pass a pin and a KeyConfig object to the underlaying Key object.
    DigitalKey(uint8_t pin, DigitalKeyConfig *config) : Key(pin, config), config(config) {}

    // The HEKeyConfig object of this digital key.
    DigitalKeyConfig *config;

    // The last time a key press on the digital key was sent, in milliseconds since firmware bootup.
    unsigned long lastDebounce = 0;

    // Bool whether the pin status on the key is currently HIGH.
    bool isHigh;
};
