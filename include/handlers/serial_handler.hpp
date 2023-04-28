#pragma once

#include "config/configuration_controller.hpp"

inline class SerialHandler
{
public:
    void handleSerialInput(String *inputStr);

private:
    void ping();
    void boot();
    void save();
    void get();
    void name(char *name);
    void out(bool state);
    void echo(char *input);
    void rt(HEKey &key, bool state);
    void crt(HEKey &key, bool state);
    void rtus(HEKey &key, uint16_t value);
    void rtds(HEKey &key, uint16_t value);
    void lh(HEKey &key, uint16_t value);
    void uh(HEKey &key, uint16_t value);
    void keyChar(HEKey &key, uint8_t value);
    void rest(HEKey &key, uint16_t value);
    void down(HEKey &key, uint16_t value);
    void hid(HEKey &key, bool state);
    bool isTrue(char *str);
    bool isEqual(char *str1, const char *str2);
} SerialHandler;
