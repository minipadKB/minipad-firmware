#pragma once

#include <cstdint>

class SMAFilter
{
public:
    SMAFilter() {}

    // Initialize the SMAFilter instance with the specified sample exponent.
    // (1 = 1 sample, 2 = 4 samples, 3 = 8 samples, ...)
    SMAFilter(uint8_t samplesExponent)
        : samplesExponent(samplesExponent)
        , samples(pow(2, samplesExponent))
        , buffer(new uint16_t[samples] {0})
    {}

    uint16_t operator()(uint16_t value);

private:
    uint8_t samplesExponent;
    uint8_t samples;
    uint16_t *buffer;
    uint8_t index = 0;
    uint32_t sum = 0;
};
