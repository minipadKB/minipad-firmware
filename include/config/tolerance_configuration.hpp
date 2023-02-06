#pragma once

#include <stdint.h>

// Configuration for the tolerances of the analog values.
struct ToleranceConfiguration
{
    // The minimum difference between the lower and upper hysteresis, as well as
    // the minimum distances for 0 to lower hysteresis and upper hysteresis to 400.
    // This tolerance prevents the key from not registering a press or a release by
    // making sure that in case the calibration is slightly off there is still a buffer.
    uint16_t hysteresisTolerance;

    // The minimum value of the rapid trigger sensitivity.
    // This prevents rapidly firing HID commands from an unstable/fluctuating analog value.
    uint16_t rapidTriggerTolerance;
};