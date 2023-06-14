#include <Arduino.h>
#include "helpers/color_helper.hpp"


uint32_t ColorHelper::hexToDec(char hex[9])
{
    // Initialize result and loop counter.
    uint32_t result = 0;
    int i = 0;

    Serial.println("2");
    Serial.printf("%d\n", result);
    strtol(hex, NULL, result);
    return result;
}

void ColorHelper::decToHex(uint32_t value, char hex[9])
{
    // Parse the ARGB uint32_t into a hex string.
    sprintf(hex, "%08X", value);
}
