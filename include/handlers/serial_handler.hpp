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
    void rt(Key &key, bool state);
    void crt(Key &key, bool state);
    void rtus(Key &key, uint16_t value);
    void rtds(Key &key, uint16_t value);
    void lh(Key &key, uint16_t value);
    void uh(Key &key, uint16_t value);
    void keyChar(Key &key, uint8_t value);
    void rest(Key &key, uint16_t value);
    void down(Key &key, uint16_t value);
    void hid(Key &key, bool state);
    bool isTrue(char *str);
    bool isEqual(char *str1, const char *str2);
} SerialHandler;