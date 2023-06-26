#include <Arduino.h>
#include "helpers/color_helper.hpp"

uint32_t ColorHelper::hexToDec(char hex[7])
{
    // Parse the hex string into an RGB uint32_t.
    uint32_t result = strtoul(hex, NULL, 16);
    return result;
}

void ColorHelper::decToHex(uint32_t value, char hex[7])
{
    // Parse the RGB uint32_t into a hex string.
    sprintf(hex, "%08X", value);
}
