#pragma once

#include "config/configuration_controller.hpp"
#include "handlers/key_states/he_key_state.hpp"

inline class SerialHandler
{
public:
    void handleSerialInput(char *input);

private:
    void boot();
    void save();
    void get();
    void name(char *name);
    void out();
    void echo(char *input);
    void hkey_rt(HEKey &key, bool state);
    void hkey_crt(HEKey &key, bool state);
    void hkey_rtus(HEKey &key, uint16_t value);
    void hkey_rtds(HEKey &key, uint16_t value);
    void hkey_lh(HEKey &key, uint16_t value);
    void hkey_uh(HEKey &key, uint16_t value);
    void key_char(Key &key, uint8_t keyChar);
    void key_hid(Key &key, bool state);
} SerialHandler;
