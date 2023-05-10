#include <Arduino.h>
#include "handlers/debug_handler.hpp"

// Define a handy macro for printing with a newline character at the end.
#define print(fmt, ...) Serial.printf(fmt "\n", __VA_ARGS__)

void DebugHandler::reportHESensorReading(const HEKey &key, uint16_t filtered, uint16_t mapped)
{
    print("[debug] (%s) key_idx: %d, filtered: %d, mapped: %d, rest: %d, down: %d", __func__, key.index, filtered, mapped, key.restPosition, key.downPosition);
}

void DebugHandler::reportDigitalReading(const DigitalKey &key, bool pressed)
{
    print("[debug] (%s) key_idx: %d, pressed: %s", __func__, key.index, pressed ? "true" : "false");
}

void DebugHandler::reportSMAFiltering(const Key &key, uint16_t raw, uint16_t filtered)
{
    print("[debug] (%s) key_idx: %d, raw: %d, filtered: %d", __func__, key.index, raw, filtered);
}

void DebugHandler::reportSerialInputGlobal(const char *input, const char *command, const char *parameters)
{
    print("[debug] (%s) input: '%s', command: '%s', parameters: '%s'", __func__, input, command, parameters);
}

void DebugHandler::reportSerialInputKey(const char *input, const char *key, const char *setting, const char *value)
{
    print("[debug] (%s) input: '%s', key: '%s', setting: '%s', value: '%s'", __func__, input, key, setting, value);
}
