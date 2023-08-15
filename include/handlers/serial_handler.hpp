#pragma once

#include "config/configuration_controller.hpp"

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
    void hkey_rt(HEKeyConfig &config, bool state);
    void hkey_crt(HEKeyConfig &config, bool state);
    void hkey_rtus(HEKeyConfig &config, uint16_t value);
    void hkey_rtds(HEKeyConfig &config, uint16_t value);
    void hkey_lh(HEKeyConfig &config, uint16_t value);
    void hkey_uh(HEKeyConfig &config, uint16_t value);
    void key_char(KeyConfig &config, uint8_t keyChar);
    void key_hid(KeyConfig &config, bool state);
} SerialHandler;
