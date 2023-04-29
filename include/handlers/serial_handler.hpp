#pragma once

#include "config/configuration_controller.hpp"

inline class SerialHandler
{
public:
    void handleSerialInput(String *inputStr);

private:
    void boot();
    void save();
    void get();
    void name(char *name);
    void out(bool state);
    void echo(char *input);
    void key_rt(HEKey &key, bool state);
    void key_crt(HEKey &key, bool state);
    void key_rtus(HEKey &key, uint16_t value);
    void key_rtds(HEKey &key, uint16_t value);
    void key_lh(HEKey &key, uint16_t value);
    void key_uh(HEKey &key, uint16_t value);
    void key_char(HEKey &key, uint8_t keyChar);
    void key_rest(HEKey &key, uint16_t value);
    void key_down(HEKey &key, uint16_t value);
    void key_hid(HEKey &key, bool state);
    void dkey_char(DigitalKey &key, uint8_t keyChar);
    void dkey_hid(DigitalKey &key, bool state);
    bool isTrue(char *str);
    bool isEqual(char *str1, const char *str2);
} SerialHandler;
