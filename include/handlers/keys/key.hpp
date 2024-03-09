#pragma once

#include <Arduino.h>
#include "config/keys/key_config.hpp"

// The base struct containing info about the state of a key for the key handler.
struct Key
{
    // Require every Key object to get an index and a KeyConfig object passed from its inheritors.
    Key(uint8_t index, KeyConfig *config) : index(index), config(config) {}

    // The index of the key. This is used to link this Key object to the corresponding KeyConfig object.
    uint8_t index;

    // The KeyConfig object of this key.
    KeyConfig *config;

    // State whether the key is currently pressed down.
    bool pressed = false;
};
