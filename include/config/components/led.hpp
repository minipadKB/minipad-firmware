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
