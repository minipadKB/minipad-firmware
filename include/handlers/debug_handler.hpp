#pragma once

#include <stdint.h>
#include "config/keys/digital_key.hpp"
#include "config/keys/he_key.hpp"
#include "config/keys/key.hpp"

inline class DebugHandler
{
public:
    void reportHESensorReading(const HEKey &key, uint16_t filtered, uint16_t mapped);
    void reportDigitalReading(const DigitalKey &key, bool presed);
    void reportSMAFiltering(const Key &key, uint16_t raw, uint16_t filtered);
    void reportSerialInputGlobal(const char* input, const char* command, const char* parameters);
    void reportSerialInputKey(const char* input, const char* key, const char* setting, const char* value);

    bool enabled = false;
private:
    void print(const char* module, const char* format, ...);
} DebugHandler;
