#pragma once

#include <stdint.h>

namespace ColorHelper
{
    uint32_t hexToDec(char hex[7]);
    void decToHex(uint32_t value, char hex[7]);
}
