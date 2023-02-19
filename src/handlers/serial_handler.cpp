#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

SerialHandler::SerialHandler(ConfigurationController *configController) : configController(configController) {}
SerialHandler::~SerialHandler() {}

void SerialHandler::handleSerialInput(String *inputStr)
{
    // Convert the string into a character array for further parsing and make it lowercase.
    char input[(*inputStr).length() + 1];
    (*inputStr).toCharArray(input, (*inputStr).length() + 1);
    Utils::toLower(input);

    // Get the first argument of the input, separated by whitespaces.
    char *arg0 = Utils::getArgumentAt(input, ' ', 0);

    // If only one argument it given (meaning the input contains no whitespace), handle the input as a normal command.
    if (strchr(input, ' ') == NULL)
    {
        handleCommand(arg0);
        return;
    }

    // If a second argument has been specified, it's an input to change a config setting.
    // It's parsed by offsetting the input pointer by the length of the first argument + the whitespace
    // as the second argument is the whole string that goes after the first argument.
    handleSet(arg0, (input + strlen(arg0) + 1));
}

// Handles the input as a command and returns a bool whether the input was handled or not.
void SerialHandler::handleCommand(char *command)
{
    // The ping command returns the firmware version and the keypads' name.
    if (isEqual(command, "ping"))
        Serial.println("pong " FIRMWARE_VERSION_FULL " | " + String(configController->config.name));

    // The reset command resets the keypad configuration back to the default without saving it.
    else if (isEqual(command, "reset"))
    {
        configController->config.keypad = configController->defaultConfig->keypad;
        Serial.println("RESET OK");
    }

    // The save command writes the full configuration to the eeprom.
    else if (isEqual(command, "save"))
    {
        configController->saveConfig();
        Serial.println("SAVE OK");
    }

    // The get command returns the values of all config values.
    else if (isEqual(command, "get"))
    {
        Serial.println("GET rt=" + String(configController->config.keypad.rapidTrigger));
        Serial.println("GET crt=" + String(configController->config.keypad.continuousRapidTrigger));
        Serial.println("GET rtsu=" + String(configController->config.keypad.rapidTriggerUpSensitivity));
        Serial.println("GET rtsd=" + String(configController->config.keypad.rapidTriggerDownSensitivity));
        Serial.println("GET lh=" + String(configController->config.keypad.lowerHysteresis));
        Serial.println("GET uh=" + String(configController->config.keypad.upperHysteresis));
        Serial.println("GET key1=" + String(configController->config.keypad.keyChars[0]));
        Serial.println("GET key2=" + String(configController->config.keypad.keyChars[1]));
#if KEYS == 3
        Serial.println("GET key3=" + String(configController->config.keypad.keyChars[2]));
#endif
        Serial.println("GET k1rp=" + String(configController->config.calibration.restPositions[0]));
        Serial.println("GET k1dp=" + String(configController->config.calibration.downPositions[0]));
        Serial.println("GET k2rp=" + String(configController->config.calibration.restPositions[1]));
        Serial.println("GET k2dp=" + String(configController->config.calibration.downPositions[1]));
#if KEYS == 3
        Serial.println("GET k3rp=" + String(configController->config.calibration.restPositions[2]));
        Serial.println("GET k3dp=" + String(configController->config.calibration.downPositions[2]));
#endif
        Serial.println("GET hid1=" + String(configController->config.keypad.hidEnabled[0]));
        Serial.println("GET hid2=" + String(configController->config.keypad.hidEnabled[1]));
#if KEYS == 3
        Serial.println("GET hid3=" + String(configController->config.keypad.hidEnabled[2]));
#endif
        Serial.println("GET tolh=" + String(HYSTERESIS_TOLERANCE));
        Serial.println("GET tolr=" + String(RAPID_TRIGGER_TOLERANCE));
        Serial.println("GET name=" + String(configController->config.name));
        Serial.println("GET END");
    }
    else
        Serial.println("Unknown command '" + String(command) + "'");
}

// Handles the input as a set command with the input being the key and the value.
void SerialHandler::handleSet(char *key, char *value)
{
    // Handle setting the keypad's name first as this is the only config setting that is not integer-related.
    if (isEqual(key, "name"))
    {
        // Make the name safe by trimming whitespaces off the name and replacing consecutive whitespaces with a single one.
        Utils::makeSafename(value);

        // Check if the name has a valid length.
        if (strlen(value) < 1 || strlen(value) > 128)
        {
            Serial.println("Invalid string length for 'name'. Expected: 1-128, Actual: " + String(strlen(value)));
            return;
        }

        // Copy the value into the name stored in the config.
        strcpy(configController->config.name, value);
        Serial.println("'name' was set to '" + String(value) + "'");

        return;
    }

    // Convert the value into a signed integer.
    int16_t valueIntSigned = atoi(value);

    // Check if the signed integer is negative and if so, ignore the command.
    // This check is done to prevent an integer overflow of an unsigned integer with negative values.
    if (valueIntSigned < 0)
        return;

    // Turn the signed integer into an unsigned one that is then used for further code.
    uint16_t valueInt = valueIntSigned;

    // If the parsed integer is different from the value string, the parsing defaults to 0.
    if (String(valueInt) != String(value))
    {
        // In that case, check if the string length is 1 meaning it was possibly a character entered
        // For simplicity of handling setting config values, the character is turned into an integer.
        if (strlen(value) == 1)
            valueInt = (uint16_t)value[0];
        // If the string could not be parsed into an integer and is not 1 character long, ignore the command.
        else
            return;
    }

    if (isEqual(key, "rt"))
    {
        // Check if the value is 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.println("Invalid value for 'rapidTrigger'. Expected: 0 or 1, Actual: " + String(valueInt));
            return;
        }

        // Set the rapid trigger setting to true or false depending on the integer entered.
        configController->config.keypad.rapidTrigger = valueInt == 1;
        Serial.println("'rapidTrigger' was set to '" + String(valueInt == 1 ? "true" : "false") + "'");
    }

    else if (isEqual(key, "crt"))
    {
        // Check if the value it 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.println("Invalid value for 'continuousRapidTrigger'. Expected: 0 or 1, Actual: " + String(valueInt));
            return;
        }

        // Set the continuous rapid trigger setting to true or false depending on the integer entered.
        configController->config.keypad.continuousRapidTrigger = valueInt == 1;
        Serial.println("'continuousRapidTrigger' was set to '" + String(valueInt == 1 ? "true" : "false") + "'");
    }

    else if (isEqual(key, "rtsu"))
    {
        // Check if the value is between the rapid trigger tolerance and 400.
        if (valueInt < RAPID_TRIGGER_TOLERANCE || valueInt > 400)
        {
            Serial.println("Invalid value for 'rapidTriggerUpSensitivity'. Expected: " + String(RAPID_TRIGGER_TOLERANCE) + "-400, Actual: " + String(valueInt));
            return;
        }

        // Set the rapid trigger up sensitivity setting to the integer entered.
        configController->config.keypad.rapidTriggerUpSensitivity = valueInt;
        Serial.println("'rapidTriggerUpSensitivity' was set to '" + String(valueInt) + "'");
    }

    else if (isEqual(key, "rtsd"))
    {
        // Check if the value is between the rapid trigger tolerance and 400.
        if (valueInt < RAPID_TRIGGER_TOLERANCE || valueInt > 400)
        {
            Serial.println("Invalid value for 'rapidTriggerDownSensitivity'. Expected: " + String(RAPID_TRIGGER_TOLERANCE) + "-400, Actual: " + String(valueInt));
            return;
        }

        // Set the rapid trigger down sensitivity setting to the integer entered.
        configController->config.keypad.rapidTriggerDownSensitivity = valueInt;
        Serial.println("'rapidTriggerDownSensitivity' was set to '" + String(valueInt) + "'");
    }

    else if (isEqual(key, "lh"))
    {
        // Check if the difference between the upper hysteresis and the new lower hysteresis is at least the hysteresis tolerance.
        if (configController->config.keypad.upperHysteresis - valueInt < HYSTERESIS_TOLERANCE)
        {
            Serial.println("Invalid value for 'lowerHysteresis'. Expected: 0-" + String(configController->config.keypad.upperHysteresis - HYSTERESIS_TOLERANCE) + ", Actual: " + String(valueInt));
            return;
        }

        // Set the lower hysteresis setting to the integer entered.
        configController->config.keypad.lowerHysteresis = valueInt;
        Serial.println("'lowerHysteresis' was set to '" + String(valueInt) + "'");
    }

    else if (isEqual(key, "uh"))
    {
        // Check if the difference between the new upper hysteresis and the lower hysteresis is at least the hysteresis tolerance.
        if (valueInt - configController->config.keypad.lowerHysteresis < HYSTERESIS_TOLERANCE)
        {
            Serial.println("Invalid value for 'upperHysteresis'. Expected: " + String(configController->config.keypad.lowerHysteresis + HYSTERESIS_TOLERANCE) + "-400, Actual: " + String(valueInt));
            return;
        }

        // Set the upper hysteresis setting to the integer entered.
        configController->config.keypad.upperHysteresis = valueInt;
        Serial.println("'upperHysteresis' was set to '" + String(valueInt) + "'");
    }

#if KEYS == 3
    else if (isEqual(key, "key1") || isEqual(key, "key2") || isEqual(key, "key3"))
#else
    else if (isEqual(key, "key1") || isEqual(key, "key2"))
#endif
    {
        // Check if the value is in the valid character range. (97-122 meaning a-z)
        if (valueInt < 97 || valueInt > 122)
        {
            Serial.println("Invalid value for '" + String(key) + "'. Expected: 97-122, Actual: " + String(valueInt));
            return;
        }

        // Set the key setting to the integer entered.
        uint8_t keyIndex = atoi(&key[3]) - 1;
        configController->config.keypad.keyChars[keyIndex] = valueInt;

        Serial.println("'" + String(valueInt) + "' was set to '" + String(valueInt) + "'");
    }

#if KEYS == 3
    else if (isEqual(key, "k1rp") || isEqual(key, "k2rp") || isEqual(key, "k3rp"))
#else
    else if (isEqual(key, "k1rp") || isEqual(key, "k2rp"))
#endif
    {
        // Get the corresponding down position of the target key to perform range comparisons.
        uint8_t keyIndex = atoi(&key[1]) - 1;
        uint16_t downPosition = configController->config.calibration.downPositions[keyIndex];

        // Check if the value is bigger than the down position and smaller or equal to 4095.
        if (valueInt <= downPosition || valueInt > 4095)
        {
            Serial.println("Invalid value for '" + String(key) + "'. Expected: " + String(downPosition + 1) + "-4095, Actual: " + String(valueInt));
            return;
        }

        // Set the key rest position setting to the integer entered.
        configController->config.calibration.restPositions[keyIndex] = valueInt;

        Serial.println("'" + String(key) + "' was set to '" + String(valueInt) + "'");
    }

#if KEYS == 3
    else if (isEqual(key, "k1dp") || isEqual(key, "k2dp") || isEqual(key, "k3dp"))
#else
    else if (isEqual(key, "k1dp") || isEqual(key, "k2dp"))
#endif
    {
        // Get the corresponding rest position of the target key to perform range comparisons.
        uint8_t keyIndex = atoi(&key[1]) - 1;
        uint16_t restPosition = configController->config.calibration.restPositions[keyIndex];

        // Check if the value is smaller than the rest positon.
        if (valueInt >= restPosition)
        {
            Serial.println("Invalid value for '" + String(key) + "'. Expected: 0-" + String(restPosition - 1) + ", Actual: " + String(valueInt));
            return;
        }

        // Set the key down position setting to the integer entered.
        configController->config.calibration.downPositions[keyIndex] = valueInt;

        Serial.println("'" + String(key) + "' was set to '" + String(valueInt) + "'");
    }

#if KEYS == 3
    else if (isEqual(key, "hid1") || isEqual(key, "hid2") || isEqual(key, "hid3"))
#else
    else if (isEqual(key, "hid1") || isEqual(key, "hid2"))
#endif
    {
        // Check if the value is 0 or 1 which is false or true.
        if (valueInt != 0 && valueInt != 1)
        {
            Serial.println("Invalid value for '" + String(key) + "'. Expected: 0 or 1, Actual: " + String(valueInt));
            return;
        }

        // Set the hid state setting to true or false depending on the integer entered.
        uint8_t keyIndex = atoi(&key[3]) - 1;
        configController->config.keypad.hidEnabled[keyIndex] = valueInt == 1;

        Serial.println("'" + String(key) + "' was set to '" + String(valueInt == 1 ? "true" : "false") + "'");
    }
}

bool SerialHandler::isEqual(char *str1, const char *str2)
{
    // Compare the specified strings and return whether they are equal or not.
    return strcmp(str1, str2) == 0;
}