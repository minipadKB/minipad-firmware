#pragma once

#include <cstdint>
#include "config/keys/key_config.hpp"

// Configuration for the digital keys of the keypad.
struct DigitalKeyConfig : KeyConfig
{
    // Default constructor for the DigitalKeyConfig struct for initializing the arrays in the Configuration struct.
    DigitalKeyConfig() : KeyConfig('\0') {}

    // Initialize with the specified key char.
    DigitalKeyConfig(char keyChar) : KeyConfig(keyChar) {}

    // This struct is empty on purpose. The only purpose it serves is explicitly having
    // a type for the digital keys, instead of differentiating between KeyConfig and DigitalKeyConfig.
};
