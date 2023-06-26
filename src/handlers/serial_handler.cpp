#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "helpers/string_helper.hpp"
#include "helpers/color_helper.hpp"
#include "definitions.hpp"
extern "C"
{
#include "pico/bootrom.h"
}

// Define a handy macro for printing with a newline character at the end.
#define print(fmt, ...) Serial.printf(fmt "\n", __VA_ARGS__)

// Define three more handy macros for interpreting the serial input.
#define isEqual(str1, str2) strcmp(str1, str2) == 0
#define isTrue(str) isEqual(str, "1") || isEqual(str, "true")
#define startsWith(str1, str2) strstr(str1, str2) == str1

void SerialHandler::handleSerialInput(String *inputStr)
{
    // Convert the string into a character array for further parsing and make it lowercase.
    char input[(*inputStr).length() + 1];
    (*inputStr).toCharArray(input, (*inputStr).length() + 1);
    StringHelper::toLower(input);

    // Parse the command as the first argument, separated by whitespaces.
    char command[1024];
    StringHelper::getArgumentAt(input, ' ', 0, command);

    // Get a pointer pointing to the start of all parameters for the command.
    char *parameters = input + strlen(command);

    // If the parameters start with a " " it means parameters have been specified.
    // It's needed because if there are no parameters there's a zero-terminator instead.
    // Skipping that zero-terminator would lead to arguments filled with memory garbage.
    if (strstr(parameters, " ") == parameters)
        parameters += 1;

    // Parse all arguments.
    char arg0[1024];
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
        out(isEqual(arg0, ""), isTrue(arg0));
#ifdef DEV
    else if (isEqual(command, "echo"))
        echo(parameters);
#endif

    // Handle hall effect key specific commands by checking if the command starts with "hkey".
    if (startsWith(command, "hkey"))
    {
        // Split the command into the key string and the setting name.
        char keyStr[1024];
        char setting[1024];
        StringHelper::getArgumentAt(command, '.', 0, keyStr);
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // By default, apply this command to all hall effect keys.
        HEKey *keys = ConfigController.config.heKeys;

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
            HEKey &key = keys[i];

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
            else if (isEqual(setting, "rest"))
                hkey_rest(key, atoi(arg0));
            else if (isEqual(setting, "down"))
                hkey_down(key, atoi(arg0));
            else if (isEqual(setting, "char"))
                key_char(key, atoi(arg0));
            else if (isEqual(setting, "hid"))
                key_hid(key, isTrue(arg0));
        }
    }

    // Handle digital key specific commands by checking if the command starts with "dkey".
    else if (startsWith(command, "dkey"))
    {
        // Split the command into the key string and the setting name.
        char keyStr[1024];
        char setting[1024];
        StringHelper::getArgumentAt(command, '.', 0, keyStr);
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // By default, apply this command to all digital keys.
        DigitalKey *keys = ConfigController.config.digitalKeys;

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
            DigitalKey &key = keys[i];

            // Handle the settings.
            if (isEqual(setting, "char"))
                key_char(key, atoi(arg0));
            else if (isEqual(setting, "hid"))
                key_hid(key, isTrue(arg0));
        }
    }
    // Handle global led-related commands by checking if the command starts with "leds.".
    // Checking for the dot at the end ensures that the identifier is "leds".
    else if (startsWith(command, "leds."))
    {
        // Get the setting name from the command.
        char setting[1024];
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // Handle the settings.
        if (isEqual(setting, "btns"))
            leds_btns(atoi(arg0));
        else if (isEqual(setting, "efct"))
            leds_efct(atoi(arg0));
    }

    // Handle led-specific commands by checking if the command starts with "led".
    else if (startsWith(command, "led"))
    {
        // Split the command into the led string and the setting name.
        char ledStr[1024];
        char setting[1024];
        StringHelper::getArgumentAt(command, '.', 0, ledStr);
        StringHelper::getArgumentAt(command, '.', 1, setting);

        // By default, apply this command to all leds.
        Led *leds = ConfigController.config.leds.leds;

        // If an index is specified ("ledX"), replace that leds array with just that led.
        // This is checked by looking whether the key string has > 3 characters.
        if (strlen(ledStr) > 3)
        {
            // Get the index and check if it's in the valid range.
            uint8_t ledIndex = atoi(ledStr + 3) - 1;
#pragma GCC diagnostic ignored "-Wtype-limits"
            if (ledIndex >= LEDS)
#pragma GCC diagnostic pop
                return;

            // Replace the array with that single digital key.
            leds = &ConfigController.config.leds.leds[ledIndex];
        }

        // Apply the command to all targetted leds.
        for (uint8_t i = 0; i < (strlen(ledStr) > 3 ? 1 : LEDS); i++)
        {
            // Get the led from the pointer array.
            Led &led = leds[i];

            // Handle the settings.
            if (isEqual(setting, "rgb"))
                led_rgb(led, arg0);
        }
    }
}

void SerialHandler::printHEKeyOutput(const HEKey &key)
{
    // Print out the index of the key, the last sensor reading and the last mapped value in the output format.
    print("OUT hkey%d=%d %d", key.index + 1, KeypadHandler.heKeyStates[key.index].lastSensorValue, KeypadHandler.heKeyStates[key.index].lastMappedValue);
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
    for (const HEKey &key : ConfigController.config.heKeys)
    {
        print("GET hkey%d.rt=%d", key.index + 1, key.rapidTrigger);
        print("GET hkey%d.crt=%d", key.index + 1, key.continuousRapidTrigger);
        print("GET hkey%d.rtus=%d", key.index + 1, key.rapidTriggerUpSensitivity);
        print("GET hkey%d.rtds=%d", key.index + 1, key.rapidTriggerDownSensitivity);
        print("GET hkey%d.lh=%d", key.index + 1, key.lowerHysteresis);
        print("GET hkey%d.uh=%d", key.index + 1, key.upperHysteresis);
        print("GET hkey%d.char=%d", key.index + 1, key.keyChar);
        print("GET hkey%d.rest=%d", key.index + 1, key.restPosition);
        print("GET hkey%d.down=%d", key.index + 1, key.downPosition);
        print("GET hkey%d.hid=%d", key.index + 1, key.hidEnabled);
    }

    // Output all digital key-specific settings.
    for (const DigitalKey &key : ConfigController.config.digitalKeys)
    {
        print("GET dkey%d.char=%d", key.index + 1, key.keyChar);
        print("GET dkey%d.hid=%d", key.index + 1, key.hidEnabled);
    }

    // Output all global led-related settings if at least one LED is registered.
    if (LEDS > 0)
    {
        print("GET leds.btns=%d", ConfigController.config.leds.brightness);
        print("GET leds.efct=%d", ConfigController.config.leds.effect);
    }

    // Output all led-specific settings.
    for (const Led &led : ConfigController.config.leds.leds)
    {
        // Parse the RGB uint16_t into a hex string.
        char hex[7];
        ColorHelper::decToHex(led.rgb, hex);

        print("GET led%d.rgb=0x%s", led.index + 1, hex);
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

void SerialHandler::out(bool single, bool state)
{
    // If single is true, no argument was specified. In that case just output every key once.
    if (single)
        for (const HEKey &key : ConfigController.config.heKeys)
            printHEKeyOutput(key);
    else
        // Otherwise, set the calibration mode field of the keypad handler to the specified state.
        KeypadHandler.outputMode = state;
}

void SerialHandler::echo(char *input)
{
    // Output the same input. This command is used for debugging purposes and only available in said environemnts.
    Serial.println(input);
}

void SerialHandler::hkey_rt(HEKey &key, bool state)
{
    // Set the rapid trigger config value to the specified state.
    key.rapidTrigger = state;
}

void SerialHandler::hkey_crt(HEKey &key, bool state)
{
    // Set the continuous rapid trigger config value to the specified state.
    key.continuousRapidTrigger = state;
}

void SerialHandler::hkey_rtus(HEKey &key, uint16_t value)
{
    // Check if the specified value is within the tolerance-TRAVEL_DISTANCE_IN_0_01MM boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= TRAVEL_DISTANCE_IN_0_01MM)
        // Set the rapid trigger up sensitivity config value to the specified state.
        key.rapidTriggerUpSensitivity = value;
}

void SerialHandler::hkey_rtds(HEKey &key, uint16_t value)
{
    // Check if the specified value is within the tolerance-TRAVEL_DISTANCE_IN_0_01MM boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= TRAVEL_DISTANCE_IN_0_01MM)
        // Set the rapid trigger down sensitivity config value to the specified state.
        key.rapidTriggerDownSensitivity = value;
}

void SerialHandler::hkey_lh(HEKey &key, uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the upper hysteresis.
    if (key.upperHysteresis - value >= HYSTERESIS_TOLERANCE)
        // Set the lower hysteresis config value to the specified state.
        key.lowerHysteresis = value;
}

void SerialHandler::hkey_uh(HEKey &key, uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the lower hysteresis.
    // Also make sure the upper hysteresis is at least said tolerance away from TRAVEL_DISTANCE_IN_0_01MM
    // to make sure the value can be reached and the key does not get stuck in an eternal pressed state.
    if (value - key.lowerHysteresis >= HYSTERESIS_TOLERANCE && TRAVEL_DISTANCE_IN_0_01MM - value >= HYSTERESIS_TOLERANCE)
        // Set the upper hysteresis config value to the specified state.
        key.upperHysteresis = value;
}

void SerialHandler::hkey_rest(HEKey &key, uint16_t value)
{
    // Check whether the specified value is bigger than the down position and smaller or equal to the maximum analog value.
    if (value > key.downPosition && value <= pow(2, ANALOG_RESOLUTION) - 1)
        // Set the rest position config value of the specified key to the specified state.
        key.restPosition = value;
}

void SerialHandler::hkey_down(HEKey &key, uint16_t value)
{
    // Check whether the specified value is smaller than the rest position.
    if (value < key.restPosition)
        // Set the down position config value of the specified key to the specified state.
        key.downPosition = value;
}

void SerialHandler::key_char(Key &key, uint8_t keyChar)
{
    // Check if the specified key is a letter with a byte value between 97 (a) and 122 (z).
    if (keyChar >= 'a' && keyChar <= 'z')
        // Set the key config value of the specified key to the specified state.
        key.keyChar = keyChar;
}

void SerialHandler::key_hid(Key &key, bool state)
{
    // Set the hid config value of the specified key to the specified state.
    key.hidEnabled = state;
}

void SerialHandler::leds_btns(uint8_t value)
{
    // Set the brightness of the LEDs to the specified value.
    ConfigController.config.leds.brightness = value;
}

void SerialHandler::leds_efct(uint8_t value)
{
    // If an out-of-bounds value was specified, default to static (0).
    if (value > LedEffectType::MaxValue)
        value = 0;

    // Set the RGB effect to the specified value.
    ConfigController.config.leds.effect = (LedEffectType)value;
}

void SerialHandler::led_rgb(Led &led, char rgb[7])
{
    // Set the rgb config value of the specified led to the specified hex value.
    led.rgb = ColorHelper::hexToDec(rgb);
}
