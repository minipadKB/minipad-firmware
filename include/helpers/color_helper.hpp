#pragma once

#include <stdint.h>

namespace ColorHelper
{
    uint32_t hexToDec(char hex[9]);
    void decToHex(uint32_t value, char hex[9]);
}
