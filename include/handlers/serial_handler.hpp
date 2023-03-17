#pragma once

#include "config/configuration_controller.hpp"

inline class SerialHandler
{
public:
    void handleSerialInput(String *inputStr);

private:
    void ping();
    void save();
    void get();
    void name(char *name);
    void out(bool state);
    void rt(bool state);
    void crt(bool state);
    void rtus(uint16_t value);
    void rtds(uint16_t value);
    void lh(uint16_t value);
    void uh(uint16_t value);
    void key(uint8_t keyIndex, uint8_t value);
    void rest(uint8_t keyIndex, uint16_t value);
    void down(uint8_t keyIndex, uint16_t value);
    void hid(uint8_t keyIndex, bool state);
    bool isTrue(char *str);
    bool isEqual(char *str1, const char *str2);
} SerialHandler;