#pragma once

#include <Arduino.h>
#include "config/keys/key_config.hpp"

// The base struct representing a key, including it's current runtime state and KeyConfig object.
struct Key
{
    // Require every Key object to get a pin and a KeyConfig object passed from its inheritors.
    Key(uint8_t pin, KeyConfig *config) : pin(pin), config(config) {}

    // The GPIO pin of the key.
    uint8_t pin;

    // The KeyConfig object of this key.
    KeyConfig *config;

    // State whether the key is currently pressed down.
    bool pressed = false;
};
