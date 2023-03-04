#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

void SerialHandler::handleSerialInput(String *inputStr)
{
    // Convert the string into a character array for further parsing and make it lowercase.
    char input[(*inputStr).length() + 1];
    (*inputStr).toCharArray(input, (*inputStr).length() + 1);
    Utils::toLower(input);

    // Get the first argument of the input, separated by whitespaces.
    char *command = Utils::getArgumentAt(input, ' ', 0);

    // The ping command returns the firmware version and the keypads' name.
    if (isEqual(command, "ping"))
    {
        Serial.print("pong " FIRMWARE_VERSION "-");
        Serial.print(KEYS);
        Serial.print("k | ");
        Serial.println(ConfigController.config.name);
    }

    // The save command writes the full configuration to the eeprom.
    else if (isEqual(command, "save"))
    {
        ConfigController.saveConfig();
        Serial.println("Config saved.");
    }

    // The cal command enables/disables the calibration mode that writes
    // the processed sensor values to the serial interface for calibration.
    else if (isEqual(command, "cal"))
    {
        KeypadHandler.calibrationMode = !KeypadHandler.calibrationMode;
        Serial.print("Calibration mode was ");
        Serial.println(KeypadHandler.calibrationMode ? "enabled." : "disabled.");
    }

    // The get command returns the values of all config values.
    else if (isEqual(command, "get"))
    {
        Serial.print("GET rt=");
        Serial.println(ConfigController.config.keypad.rapidTrigger == 1 ? "true" : "false");
        Serial.print("GET crt=");
        Serial.println(ConfigController.config.keypad.continuousRapidTrigger == 1 ? "true" : "false");
        Serial.print("GET rtus=");
        Serial.println(ConfigController.config.keypad.rapidTriggerUpSensitivity);
        Serial.print("GET rtds=");
        Serial.println(ConfigController.config.keypad.rapidTriggerDownSensitivity);
        Serial.print("GET lh=");
        Serial.println(ConfigController.config.keypad.lowerHysteresis);
        Serial.print("GET uh=");
        Serial.println(ConfigController.config.keypad.upperHysteresis);

        // Output all key-specific config values.
        for (uint8_t keyIndex = 0; keyIndex < KEYS; keyIndex++)
        {
            Serial.print("GET key");
            Serial.print(keyIndex + 1);
            Serial.print(".key=");
            Serial.println(ConfigController.config.keypad.keyChars[keyIndex]);

            Serial.print("GET key");
            Serial.print(keyIndex + 1);
            Serial.print(".rest=");
            Serial.println(ConfigController.config.calibration.restPositions[keyIndex]);

            Serial.print("GET key");
            Serial.print(keyIndex + 1);
            Serial.print(".down=");
            Serial.println(ConfigController.config.calibration.downPositions[keyIndex]);

            Serial.print("GET key");
            Serial.print(keyIndex + 1);
            Serial.print(".hid=");
            Serial.println(ConfigController.config.keypad.hidEnabled[keyIndex] == 1 ? "true" : "false");
        }

        Serial.print("GET tolh=");
        Serial.println(HYSTERESIS_TOLERANCE);
        Serial.print("GET tolr=");
        Serial.println(RAPID_TRIGGER_TOLERANCE);
        Serial.print("GET name=");
        Serial.println(ConfigController.config.name);
        Serial.println("GET END");
    }

    // Check if the command starts with "key", e.g. "key1", "key2", ...
    else if (strstr(command, "key") == command)
    {
        // Get the index and check if it's in the valid range.
        uint8_t keyIndex = atoi(command + 3) - 1;
        if (keyIndex >= KEYS)

            return;

        // If the command is a valid key command (e.g. key1, key2, ...), it's an input to change a config setting.
        // It's parsed by offsetting the input pointer by the length of the key command + a whitespace
        // + setting name + whitespace as the setting value is the whole string that goes after the second argument.
        char *setting = Utils::getArgumentAt(input, ' ', 1);
        handleKeySetting(keyIndex, setting, (input + strlen(command) + 1 + strlen(setting) + 1));
    }

    // Otherwise handle it as a global setting by passing the command and value which is
    // parsed by offsetting the input pointer by the length of the command + a whitespace.
    else
        handleGlobalSetting(command, input + strlen(command) + 1);
}

void SerialHandler::handleGlobalSetting(char *setting, char *value)
{
    // Handle setting the keypad's name first as this is the only config setting that is not integer-related.
    if (isEqual(setting, "name"))
    {
        // Make the name safe by trimming whitespaces off the name and replacing consecutive whitespaces with a single one.
        Utils::makeSafename(value);

        // Check if the name has a valid length.
        if (strlen(value) < 1 || strlen(value) > 128)
        {
            Serial.print("Invalid string length for 'name'. Expected: 1-128, Actual: ");
            Serial.println(strlen(value));
            return;
        }

        // Copy the value into the name stored in the config.
        strcpy(ConfigController.config.name, value);
        Serial.print("'name' was set to '");
        Serial.print(value);
        Serial.println("'");
    }

    // Convert the value into an integer for all settings from here on.
    uint16_t valueInt = atoi(value);

    if (isEqual(setting, "rt"))
    {
        // Check if the value is 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.print("Invalid value for 'rapidTrigger'. Expected: 0 or 1, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the rapid trigger setting to true or false depending on the integer entered.
        ConfigController.config.keypad.rapidTrigger = valueInt == 1;
        Serial.print("'rapidTrigger' was set to '");
        Serial.print(valueInt == 1 ? "true" : "false");
        Serial.println("'");
    }

    else if (isEqual(setting, "crt"))
    {
        // Check if the value it 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.print("Invalid value for 'continuousRapidTrigger'. Expected: 0 or 1, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the continuous rapid trigger setting to true or false depending on the integer entered.
        ConfigController.config.keypad.continuousRapidTrigger = valueInt == 1;
        Serial.print("'continuousRapidTrigger' was set to '");
        Serial.print(valueInt == 1 ? "true" : "false");
        Serial.println("'");
    }

    else if (isEqual(setting, "rtus"))
    {
        // Check if the value is between the rapid trigger tolerance and 400.
        if (valueInt < RAPID_TRIGGER_TOLERANCE || valueInt > 400)
        {
            Serial.print("Invalid value for 'rapidTriggerUpSensitivity'. Expected: ");
            Serial.print(RAPID_TRIGGER_TOLERANCE);
            Serial.print("-400, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the rapid trigger up sensitivity setting to the integer entered.
        ConfigController.config.keypad.rapidTriggerUpSensitivity = valueInt;
        Serial.print("'rapidTriggerUpSensitivity' was set to '");
        Serial.print(valueInt);
        Serial.println("'");
    }

    else if (isEqual(setting, "rtds"))
    {
        // Check if the value is between the rapid trigger tolerance and 400.
        if (valueInt < RAPID_TRIGGER_TOLERANCE || valueInt > 400)
        {
            Serial.println("Invalid value for 'rapidTriggerDownSensitivity'. Expected: ");
            Serial.print(RAPID_TRIGGER_TOLERANCE);
            Serial.print("-400, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the rapid trigger down sensitivity setting to the integer entered.
        ConfigController.config.keypad.rapidTriggerDownSensitivity = valueInt;
        Serial.print("'rapidTriggerDownSensitivity' was set to '");
        Serial.print(valueInt);
        Serial.println("'");
    }

    else if (isEqual(setting, "lh"))
    {
        // Check if the difference between the upper hysteresis and the new lower hysteresis is at least the hysteresis tolerance.
        if (ConfigController.config.keypad.upperHysteresis - valueInt < HYSTERESIS_TOLERANCE)
        {
            Serial.print("Invalid value for 'lowerHysteresis'. Expected: 0-");
            Serial.print(ConfigController.config.keypad.upperHysteresis - HYSTERESIS_TOLERANCE);
            Serial.print(", Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the lower hysteresis setting to the integer entered.
        ConfigController.config.keypad.lowerHysteresis = valueInt;
        Serial.println("'lowerHysteresis' was set to '" + String(valueInt) + "'");
    }

    else if (isEqual(setting, "uh"))
    {
        // Check if the difference between the new upper hysteresis and the lower hysteresis is at least the hysteresis tolerance.
        if (valueInt - ConfigController.config.keypad.lowerHysteresis < HYSTERESIS_TOLERANCE)
        {
            Serial.println("Invalid value for 'upperHysteresis'. Expected: " + String(ConfigController.config.keypad.lowerHysteresis + HYSTERESIS_TOLERANCE) + "-400, Actual: " + String(valueInt));
            return;
        }

        // Set the upper hysteresis setting to the integer entered.
        ConfigController.config.keypad.upperHysteresis = valueInt;
        Serial.println("'upperHysteresis' was set to '" + String(valueInt) + "'");
    }
}

// Handles the input as a set command with the input being the keyIndex, setting and the value.
void SerialHandler::handleKeySetting(uint8_t keyIndex, char *setting, char *value)
{
    // Convert the value into an integer for all settings from here on.
    uint16_t valueInt = atoi(value);

    if (isEqual(setting, "key"))
    {
        // Check if the value is in the valid character range. (97-122 meaning a-z)
        if (valueInt < 97 || valueInt > 122)
        {
            Serial.print("Invalid value for 'key");
            Serial.print(keyIndex + 1);
            Serial.print(".");
            Serial.print(setting);
            Serial.print("'. Expected: 97-122, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the key setting to the integer entered.
        ConfigController.config.keypad.keyChars[keyIndex] = valueInt;

        Serial.print("'key");
        Serial.print(keyIndex + 1);
        Serial.print(".key' was set to '");
        Serial.print(valueInt);
        Serial.println("'");
    }

    else if (isEqual(setting, "rest"))
    {
        // Get the corresponding down position of the target key to perform range comparisons.
        uint16_t downPosition = ConfigController.config.calibration.downPositions[keyIndex];

        // Check if the value is bigger than the down position and smaller or equal to 4095.
        if (valueInt <= downPosition || valueInt > 4095)
        {
            Serial.print("Invalid value for 'key");
            Serial.print(keyIndex + 1);
            Serial.print(".rest'. Expected: ");
            Serial.print(downPosition + 1);
            Serial.print("-4095, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the key rest position setting to the integer entered.
        ConfigController.config.calibration.restPositions[keyIndex] = valueInt;

        Serial.print("'key");
        Serial.print(keyIndex + 1);
        Serial.print(".rest' was set to '");
        Serial.print(valueInt);
        Serial.println("'");
    }

    else if (isEqual(setting, "down"))
    {
        // Get the corresponding rest position of the target key to perform range comparisons.
        uint16_t restPosition = ConfigController.config.calibration.restPositions[keyIndex];

        // Check if the value is smaller than the rest positon.
        if (valueInt >= restPosition)
        {
            Serial.print("Invalid value for 'key");
            Serial.print(keyIndex + 1);
            Serial.print(".down'. Expected: 0-");
            Serial.print(restPosition - 1);
            Serial.print(", Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the key down position setting to the integer entered.
        ConfigController.config.calibration.downPositions[keyIndex] = valueInt;

        Serial.print("'key");
        Serial.print(keyIndex + 1);
        Serial.print(".down' was set to '");
        Serial.print(valueInt);
        Serial.println("'");
    }

    else if (isEqual(setting, "hid"))
    {
        // Check if the value is 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.print("Invalid value for 'key");
            Serial.print(keyIndex + 1);
            Serial.print(".hid'. Expected: 0 or 1, Actual: ");
            Serial.println(valueInt);
            return;
        }

        // Set the hid state setting to true or false depending on the integer entered.
        ConfigController.config.keypad.hidEnabled[keyIndex] = valueInt == 1;

        Serial.print("'key");
        Serial.print(keyIndex + 1);
        Serial.print(".hid' was set to '");
        Serial.print(valueInt == 1 ? "true" : "false");
        Serial.println("'");
    }
}

bool SerialHandler::isEqual(char *str1, const char *str2)
{
    // Compare the specified strings and return whether they are equal or not.
    return strcmp(str1, str2) == 0;
}