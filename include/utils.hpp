#pragma once
#include <Arduino.h>

namespace Utils
{
    char *getArgumentAt(char *input, char delimiter, uint8_t index);
    void toLower(char *input);
    void replace(char *input, char target, char replacement);
    void makeSafename(char *str);
    char *substring(char *input, int offset, int length);
};