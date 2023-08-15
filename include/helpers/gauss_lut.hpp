#pragma once

#include <cstdint>
#include "definitions.hpp"

class GaussLUT
{
public:
    // Variables for the equation to calculate the ADC reading into a physical distance.
    // a = y-stretch, b = x-stretch, c = x-offset, d = y-offset, for more info: https://www.desmos.com/calculator/ps4wd127tu
    GaussLUT(double a, double b, double c, double d);

    uint16_t adcToDistance(const uint16_t adc, uint16_t const restPosition);

private:
    // The calculated lookup table used by this GaussLUT instance.
    uint16_t lut[1 << ANALOG_RESOLUTION];

    // The rest position of the keys according to the lookup table.
    uint16_t lutRestPosition;
};
