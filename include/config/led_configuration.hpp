#pragma once

#include "config/components/led.hpp"

// Configuration for the LEDs, containing the actual LEDs, brightness, effects, ... of the keypad.
struct LedConfiguration
{
    // A list of all LEDs. (rgb, effect, ...)
    Led leds[LEDS];

    // The brightness of the LEDs. (0-100)
    uint8_t brightness;
};
