#pragma once

#include <stdint.h>

// Configuration for the LEDs registered on the hardware.
struct Led
{
    // The index of the led. This is hardcoded in the default config and is not changed.
    // It does not serve a config purpose but is instead for accessing the index from the Led object.
    uint8_t index;

    // The RGB color of the led.
    uint32_t rgb;
};

// An enum used to identify the type of effect the LEDs have.
enum LedEffectType
{
    // The configured RGB colors are shown at the configured brightness statically.
    Static,

    // The configured RGB color is shown with brightness dependant on how far the furthest key is pressed in.
    Analog,

    // Last value used to identify the end of the enum. This is necessary to get the amount of elements in the enum.
    // e.g. in the serial command, there is a check looking whether the specified value is smaller than this.
    MaxValue
};
