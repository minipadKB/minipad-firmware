#include <Arduino.h>
#include "handlers/keys/he_key.hpp"
#include "handlers/serial_handler.hpp"
#include "handlers/key_handler.hpp"
#include "helpers/string_helper.hpp"
#include "definitions.hpp"
extern "C"
{
#include "pico/bootrom.h"
}

// Define a handy macro for printing with a newline character at the end.
#define print(fmt, ...) Serial.printf(fmt "\n", __VA_ARGS__)

// Define two more handy macros for interpreting the serial input.
#define isEqual(str1, str2) strcmp(str1, str2) == 0
#define isTrue(str) isEqual(str, "1") || isEqual(str, "true")

void SerialHandler::handleSerialInput(char *input)
{
    // Make the input buffer lowercase for further parsing.
    StringHelper::toLower(input);

    // Parse the command as the first argument, separated by whitespaces.
    char command[SERIAL_INPUT_BUFFER_SIZE];
    StringHelper::getArgumentAt(input, ' ', 0, command);

    // Get a pointer pointing to the start of all parameters for the command.
    char *parameters = input + strlen(command);

    // If the parameters start with a " " it means parameters have been specified.
    // It's needed because if there are no parameters there's a zero-terminator instead.
    // Skipping that zero-terminator would lead to arguments filled with memory garbage.
    if (strstr(parameters, " ") == parameters)
        parameters += 1;

    // Parse all arguments.
    char arg0[SERIAL_INPUT_BUFFER_SIZE];
    StringHelper::getArgumentAt(parameters, ' ', 0, arg0);

    // Handle the global commands and pass their expected required parameters.
    if (isEqual(command, "boot"))
        boot();
    else if (isEqual(command, "save"))
        save();
    else if (isEqual(command, "get"))
        get();
    else if (isEqual(command, "name"))
        name(parameters);
    else if (isEqual(command, "out"))
        out();
#ifdef DEV
    else if (isEqual(command, "echo"))
        echo(parameters);
#endif

    // Handle hall effect key specific commands by checking if the command starts with "hkey".
    if (strstr(command, "hkey") == command)
    {
        // Split the command into the key string and the setting name.
        char keyStr[SERIAL_INPUT_BUFFER_SIZE];
        char setting[SERIAL_INPUT_BUFFER_SIZE];
        StringHelper::getArgumentAt(command, '.', 0, keyStr);
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // By default, apply this command to all hall effect keys.
        HEKeyConfig *keys = ConfigController.config.heKeys;

        // If an index is specified ("hkeyX"), replace that keys array with just that key.
        // This is checked by looking whether the key string has > 4 characters.
        if (strlen(keyStr) > 4)
        {
            // Get the index and check if it's in the valid range.
            uint8_t keyIndex = atoi(keyStr + 4) - 1;
            if (keyIndex >= HE_KEYS)
                return;

            // Replace the array with that single key.
            keys = &ConfigController.config.heKeys[keyIndex];
        }

        // Apply the command to all targetted hall effect keys.
        for (uint8_t i = 0; i < (strlen(keyStr) > 4 ? 1 : HE_KEYS); i++)
        {
            // Get the key from the pointer array.
            HEKeyConfig &key = keys[i];

            // Handle the settings.
            if (isEqual(setting, "rt"))
                hkey_rt(key, isTrue(arg0));
            else if (isEqual(setting, "crt"))
                hkey_crt(key, isTrue(arg0));
            else if (isEqual(setting, "rtus"))
                hkey_rtus(key, atoi(arg0));
            else if (isEqual(setting, "rtds"))
                hkey_rtds(key, atoi(arg0));
            else if (isEqual(setting, "lh"))
                hkey_lh(key, atoi(arg0));
            else if (isEqual(setting, "uh"))
                hkey_uh(key, atoi(arg0));
            else if (isEqual(setting, "char"))
                key_char(key, strlen(arg0) == 1 ? (int)arg0[0] : atoi(arg0) /* Allow for either the ASCII character or integer */);
            else if (isEqual(setting, "hid"))
                key_hid(key, isTrue(arg0));
        }
    }

    // Handle digital key specific commands by checking if the command starts with "dkey".
    if (strstr(command, "dkey") == command)
    {
        // Split the command into the key string and the setting name.
        char keyStr[SERIAL_INPUT_BUFFER_SIZE];
        char setting[SERIAL_INPUT_BUFFER_SIZE];
        StringHelper::getArgumentAt(command, '.', 0, keyStr);
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // By default, apply this command to all digital keys.
        DigitalKeyConfig *keys = ConfigController.config.digitalKeys;

        // If an index is specified ("dkeyX"), replace that keys array with just that key.
        // This is checked by looking whether the key string has > 4 characters.
        if (strlen(keyStr) > 4)
        {
            // Get the index and check if it's in the valid range.
            uint8_t keyIndex = atoi(keyStr + 4) - 1;
#pragma GCC diagnostic ignored "-Wtype-limits"
            if (keyIndex >= DIGITAL_KEYS)
#pragma GCC diagnostic pop
                return;

            // Replace the array with that single digital key.
            keys = &ConfigController.config.digitalKeys[keyIndex];
        }

        // Apply the command to all targetted digital keys.
        for (uint8_t i = 0; i < (strlen(keyStr) > 4 ? 1 : DIGITAL_KEYS); i++)
        {
            // Get the key from the pointer array.
            DigitalKeyConfig &key = keys[i];

            // Handle the settings.
            if (isEqual(setting, "char"))
                key_char(key, strlen(arg0) == 1 ? (int)arg0[0] : atoi(arg0) /* Allow for either the ASCII character or integer */);
            else if (isEqual(setting, "hid"))
                key_hid(key, isTrue(arg0));
        }
    }
}

void SerialHandler::boot()
{
    // Set the RP2040 into bootloader mode.
    reset_usb_boot(0, 0);
}

void SerialHandler::save()
{
    // Save the configuration managed by the config controller.
    ConfigController.saveConfig();
}

void SerialHandler::get()
{
    // Output all global settings.
    print("GET version=%s%s", FIRMWARE_VERSION, DEV ? "-dev" : "");
    print("GET hkeys=%d", HE_KEYS);
    print("GET dkeys=%d", DIGITAL_KEYS);
    print("GET name=%s", ConfigController.config.name);
    print("GET htol=%d", HYSTERESIS_TOLERANCE);
    print("GET rtol=%d", RAPID_TRIGGER_TOLERANCE);
    print("GET trdt=%d", TRAVEL_DISTANCE_IN_0_01MM);
    print("GET ares=%d", ANALOG_RESOLUTION);

    // Output all hall effect key-specific settings.
    for (const HEKey &key : KeyHandler.heKeys)
    {
        // Format the base for all lines being written.
        print("GET hkey%d.rt=%d", key.index + 1, key.config->rapidTrigger);
        print("GET hkey%d.crt=%d", key.index + 1, key.config->continuousRapidTrigger);
        print("GET hkey%d.rtus=%d", key.index + 1, key.config->rapidTriggerUpSensitivity);
        print("GET hkey%d.rtds=%d", key.index + 1, key.config->rapidTriggerDownSensitivity);
        print("GET hkey%d.lh=%d", key.index + 1, key.config->lowerHysteresis);
        print("GET hkey%d.uh=%d", key.index + 1, key.config->upperHysteresis);
        print("GET hkey%d.char=%d", key.index + 1, key.config->keyChar);
        print("GET hkey%d.hid=%d", key.index + 1, key.config->hidEnabled);
        print("GET hkey%d.rest=%d", key.index + 1, key.restPosition);
        print("GET hkey%d.down=%d", key.index + 1, key.downPosition);
    }

    // Output all digital key-specific settings.
    for (const DigitalKey &key : KeyHandler.digitalKeys)
    {
        print("GET dkey%d.char=%d", key.index + 1, key.config->keyChar);
        print("GET dkey%d.hid=%d", key.index + 1, key.config->hidEnabled);
    }

    // Print this line to signalize the end of printing the settings to the listener.
    Serial.println("GET END");
}

void SerialHandler::name(char *name)
{
    // Get the length of the name and check if it's within the 1-128 characters boundary.
    size_t length = strlen(name);
    if (length >= 1 && length <= 128)
        memcpy(ConfigController.config.name, name + '\0', length + 1);
}

void SerialHandler::out()
{
    // Output the raw sensor value and magnet distance of every Hall Effect key once.
    for (const HEKey &key : KeyHandler.heKeys)
        print("OUT hkey%d=%d %d", key.index + 1, key.rawValue, key.distance);
}

void SerialHandler::echo(char *input)
{
    // Output the same input. This command is used for debugging purposes and only available in said environemnts.
    Serial.println(input);
}

void SerialHandler::hkey_rt(HEKeyConfig &config, bool state)
{
    // Set the rapid trigger config value to the specified state.
    config.rapidTrigger = state;
}

void SerialHandler::hkey_crt(HEKeyConfig &config, bool state)
{
    // Set the continuous rapid trigger config value to the specified state.
    config.continuousRapidTrigger = state;
}

void SerialHandler::hkey_rtus(HEKeyConfig &config, uint16_t value)
{
    // Check if the specified value is within the tolerance-TRAVEL_DISTANCE_IN_0_01MM boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= TRAVEL_DISTANCE_IN_0_01MM)
        // Set the rapid trigger up sensitivity config value to the specified state.
        config.rapidTriggerUpSensitivity = value;
}

void SerialHandler::hkey_rtds(HEKeyConfig &config, uint16_t value)
{
    // Check if the specified value is within the tolerance-TRAVEL_DISTANCE_IN_0_01MM boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= TRAVEL_DISTANCE_IN_0_01MM)
        // Set the rapid trigger down sensitivity config value to the specified state.
        config.rapidTriggerDownSensitivity = value;
}

void SerialHandler::hkey_lh(HEKeyConfig &config, uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the upper hysteresis.
    if (config.upperHysteresis - value >= HYSTERESIS_TOLERANCE)
        // Set the lower hysteresis config value to the specified state.
        config.lowerHysteresis = value;
}

void SerialHandler::hkey_uh(HEKeyConfig &config, uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the lower hysteresis.
    // Also make sure the upper hysteresis is at least said tolerance away from TRAVEL_DISTANCE_IN_0_01MM
    // to make sure the value can be reached and the key does not get stuck in an eternal pressed state.
    if (value - config.lowerHysteresis >= HYSTERESIS_TOLERANCE && TRAVEL_DISTANCE_IN_0_01MM - value >= HYSTERESIS_TOLERANCE)
        // Set the upper hysteresis config value to the specified state.
        config.upperHysteresis = value;
}

void SerialHandler::key_char(KeyConfig &config, uint8_t keyChar)
{
    // Set the key config value of the specified key to the specified state.
    config.keyChar = keyChar;
}

void SerialHandler::key_hid(KeyConfig &config, bool state)
{
    // Set the hid config value of the specified key to the specified state.
    config.hidEnabled = state;
}
