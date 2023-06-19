#pragma once

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
