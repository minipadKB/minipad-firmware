#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "helpers/string_helper.hpp"
#include "definitions.hpp"

// Define a handy macro for printing with a newline character at the end.
#define print(fmt, ...) Serial.printf(fmt "\n", __VA_ARGS__)

void SerialHandler::handleSerialInput(String *inputStr)
{
    // Convert the string into a character array for further parsing and make it lowercase.
    char input[(*inputStr).length() + 1];
    (*inputStr).toCharArray(input, (*inputStr).length() + 1);
    StringHelper::toLower(input);

    // Get the first argument of the input, separated by whitespaces.
    char *command = StringHelper::getArgumentAt(input, ' ', 0);
    char *parameters = input + strlen(command) + 1;
    char *arg0 = StringHelper::getArgumentAt(parameters, ' ', 0);

    // Handle the global commands and pass their expected required parameters.
    if (isEqual(command, "ping"))
        ping();
    else if (isEqual(command, "save"))
        save();
    else if (isEqual(command, "get"))
        get();
    else if (isEqual(command, "name"))
        name(parameters);
    else if (isEqual(command, "out"))
        out(isTrue(arg0));
    else if (isEqual(command, "rt"))
        rt(isTrue(arg0));
    else if (isEqual(command, "crt"))
        crt(isTrue(arg0));
    else if (isEqual(command, "rtus"))
        rtus(atoi(arg0));
    else if (isEqual(command, "rtds"))
        rtds(atoi(arg0));
    else if (isEqual(command, "lh"))
        lh(atoi(arg0));
    else if (isEqual(command, "uh"))
        uh(atoi(arg0));

    // Handle key specific commands.
    if (strstr(command, "key") == command)
    {
        // Get the index and check if it's in the valid range.
        uint8_t keyIndex = atoi(command + 3) - 1;
        if (keyIndex >= KEYS)
            return;

        char *setting = StringHelper::getArgumentAt(command, '.', 1);

        // Handle the settings.
        if (isEqual(setting, "key"))
            key(keyIndex, atoi(arg0));
        if (isEqual(setting, "rest"))
            rest(keyIndex, atoi(arg0));
        if (isEqual(setting, "down"))
            down(keyIndex, atoi(arg0));
        if (isEqual(setting, "hid"))
            hid(keyIndex, isTrue(arg0));
    }
}

void SerialHandler::ping()
{
    // Print out the pong message including the firmware version and the name of the keypad.
    print("pong %s-%dk | %s\n", FIRMWARE_VERSION, KEYS, ConfigController.config.name);
}

void SerialHandler::save()
{
    // Save the configuration managed by the config controller.
    ConfigController.saveConfig();
}

void SerialHandler::get()
{
    // Output all global settings.
    print("GET rt=%d", ConfigController.config.keypad.rapidTrigger);
    print("GET crt=%d", ConfigController.config.keypad.continuousRapidTrigger);
    print("GET rtus=%d", ConfigController.config.keypad.rapidTriggerUpSensitivity);
    print("GET rtds=%d", ConfigController.config.keypad.rapidTriggerDownSensitivity);
    print("GET lh=%d", ConfigController.config.keypad.lowerHysteresis);
    print("GET uh=%d", ConfigController.config.keypad.upperHysteresis);

    // Output all key-specific settings.
    for (uint8_t keyIndex = 0; keyIndex < KEYS; keyIndex++)
    {
        // Format the base for all lines being written.
        char base[10];
        sprintf(base, "GET key%d", keyIndex + 1);
        print("%s.key=%d", base, ConfigController.config.keypad.keyChars[keyIndex]);
        print("%s.rest=%d", base, ConfigController.config.calibration.restPositions[keyIndex]);
        print("%s.down=%d", base, ConfigController.config.calibration.downPositions[keyIndex]);
        print("%s.hid=%d", base, ConfigController.config.keypad.hidEnabled[keyIndex]);
    }
}

void SerialHandler::name(char *name)
{
    // Get the length of the name and check if it's within the 1-128 characters boundary.
    size_t length = strlen(name);
    if (length >= 1 && length <= 128)
        strcpy(ConfigController.config.name, name);
}

void SerialHandler::out(bool state)
{
    // Set the calibration mode field of the keypad handler to the specified state.
    KeypadHandler.calibrationMode = state;
}

void SerialHandler::rt(bool state)
{
    // Set the rapid trigger config value to the specified state.
    ConfigController.config.keypad.rapidTrigger = state;
}

void SerialHandler::crt(bool state)
{
    // Set the continuous rapid trigger config value to the specified state.
    ConfigController.config.keypad.continuousRapidTrigger = state;
}

void SerialHandler::rtus(uint16_t value)
{
    // Check if the specified value is within the tolerance-400 boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= 400)
        // Set the rapid trigger up sensitivity config value to the specified state.
        ConfigController.config.keypad.rapidTriggerUpSensitivity = value;
}

void SerialHandler::rtds(uint16_t value)
{
    // Check if the specified value is within the tolerance-400 boundary.
    if (value >= RAPID_TRIGGER_TOLERANCE && value <= 400)
        // Set the rapid trigger down sensitivity config value to the specified state.
        ConfigController.config.keypad.rapidTriggerDownSensitivity = value;
}

void SerialHandler::lh(uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the upper hysteresis.
    if (ConfigController.config.keypad.upperHysteresis - value >= HYSTERESIS_TOLERANCE)
        // Set the lower hysteresis config value to the specified state.
        ConfigController.config.keypad.lowerHysteresis = value;
}

void SerialHandler::uh(uint16_t value)
{
    // Check if the specified value is at least the hysteresis tolerance away from the lower hysteresis.
    // Also make sure the upper hysteresis is at least said tolerance away from 400 to make sure it can be reached.
    if (value - ConfigController.config.keypad.lowerHysteresis >= HYSTERESIS_TOLERANCE && 400 - value >= HYSTERESIS_TOLERANCE)
        // Set the upper hysteresis config value to the specified state.
        ConfigController.config.keypad.upperHysteresis = value;
}

void SerialHandler::key(uint8_t keyIndex, uint8_t key)
{
    // Check if the specified key is a letter with a byte value between 97 and 122.
    if (key >= 97 && key <= 122)
        // Set the key config value of the specified key to the specified state.
        ConfigController.config.keypad.keyChars[keyIndex] = key;
}

void SerialHandler::rest(uint8_t keyIndex, uint16_t value)
{
    // Check whether the specified value is bigger than the down position and smaller or equal to the maximum analog value.
    if (value > ConfigController.config.calibration.downPositions[keyIndex] && value <= pow(2, ANALOG_RESOLUTION) - 1)
        // Set the rest position config value of the specified key to the specified state.
        ConfigController.config.calibration.restPositions[keyIndex] = value;
}

void SerialHandler::down(uint8_t keyIndex, uint16_t value)
{
    // Check whether the specified value is smaller than the rest position.
    if (value < ConfigController.config.calibration.restPositions[keyIndex])
        // Set the down position config value of the specified key to the specified state.
        ConfigController.config.calibration.downPositions[keyIndex] = value;
}

void SerialHandler::hid(uint8_t keyIndex, bool state)
{
    // Set the hid config value of the specified key to the specified state.
    ConfigController.config.keypad.hidEnabled[keyIndex] = state;
}

bool SerialHandler::isTrue(char *str)
{
    return isEqual(str, "1") || isEqual(str, "true");
}

bool SerialHandler::isEqual(char *str1, const char *str2)
{
    // Compare the specified strings and return whether they are equal or not.
    return strcmp(str1, str2) == 0;
}