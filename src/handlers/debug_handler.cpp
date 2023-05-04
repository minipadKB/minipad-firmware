#include <Arduino.h>
#include "handlers/debug_handler.hpp"

void DebugHandler::print(const char *module, const char *format, ...)
{
    // Do not print anything if the debug output is not enabled.
    if (!enabled)
        return;

    // Get the variadic list of parameters.
    va_list args;
    va_start(args, format);

    // Prepare the output with it's debug format and print it to the serial interface.
    char output[1024];
    sprintf(output, "[debug] (%s) %s\n", module, format);
    Serial.printf(output, args);
    va_end(args);
}

void DebugHandler::reportHESensorReading(const HEKey &key, uint16_t filtered, uint16_t mapped)
{
    print("HESensorReading", "key_idx: %d, filtered: %d, mapped: %d, rest: %d, down: %d", key.index, filtered, mapped, key.restPosition, key.downPosition);
}

void DebugHandler::reportDigitalReading(const DigitalKey &key, bool pressed)
{
    print("DigitalReading", "key_idx: %d, pressed: %s", key.index, pressed ? "true" : "false");
}

void DebugHandler::reportSMAFiltering(const Key &key, uint16_t raw, uint16_t filtered)
{
    print("SMAFiltering", "key_idx: %d, raw: %d, filtered: %d", key.index, raw, filtered);
}

void DebugHandler::reportSerialInputGlobal(const char *input, const char *command, const char *parameters)
{
    print("SerialInput", "input: '%s', command: '%s', parameters: '%s'", input, command, parameters);
}

void DebugHandler::reportSerialInputKey(const char *input, const char *key, const char *setting, const char *value)
{
    print("SerialInput", "input: '%s', key: '%s', setting: '%s', value: '%s'", key, setting, value);
}
