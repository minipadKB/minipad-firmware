#include <Arduino.h>
#include "helpers/gauss_lut.hpp"
#include "definitions.hpp"

GaussLUT::GaussLUT(double a, double b, double c, double d)
{
    // Fill the range from a to d in the LUT based on the parameters and the equation. (See:https://www.desmos.com/calculator/ps4wd127tu)
    // This calculates the "ideal" distance based on the relevant ADC range, being from a to d, since everything above a - d will equal to 0, anyways.
    for (uint16_t i = 0; i < a - d; i++)
            lut[i] = constrain(((log(1 - ((i + d) / a)) / -b) - c), 0, TRAVEL_DISTANCE_IN_0_01MM);

    // Calculate the "ideal" rest position of the LUT to calculate offsets on real-based rest positions later on.
    lutRestPosition = a * (1 - exp(-b * c)) - d;
}

uint16_t GaussLUT::adcToDistance(const uint16_t adc, const uint16_t restPosition)
{
    // Get the offset by the difference between the "ideal" rest position of the LUT and the one of the sensor.
    int16_t offset = lutRestPosition - restPosition;

    // Return the value at the index of the adc value, shifted by the offset determined above.
    return lut[adc + offset];
}
