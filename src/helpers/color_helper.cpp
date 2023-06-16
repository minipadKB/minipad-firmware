#include <Arduino.h>
#include "helpers/color_helper.hpp"

uint32_t ColorHelper::hexToDec(char hex[9])
{
    // Parse the hex string into an ARGB uint32_t.
    uint32_t result = strtoul(hex, NULL, 16);
    return result;
}

void ColorHelper::decToHex(uint32_t value, char hex[9])
{
    // Parse the ARGB uint32_t into a hex string.
    sprintf(hex, "%08X", value);
}
