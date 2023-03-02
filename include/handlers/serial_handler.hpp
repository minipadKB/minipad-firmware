#pragma once

#include "config/configuration_controller.hpp"

inline class SerialHandler
{
public:
    void handleSerialInput(String *inputStr);

private:
    void handleGlobalSetting(char *key, char *value);
    void handleKeySetting(uint8_t keyIndex, char *key, char *value);
    bool isEqual(char *str1, const char *str2);
} SerialHandler;