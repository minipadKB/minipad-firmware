#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

SerialHandler::SerialHandler(ConfigurationController *configController, char *firmwareVersion) : configController(configController), firmwareVersion(firmwareVersion) {}
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

// Handles the input as a command and returns a bool whether the input was handled or not
void SerialHandler::handleCommand(char *command)
{
  if (isEqual(command, "ping"))
    Serial.println("pong " + String(firmwareVersion) + " | " + configController->config.name);
  else if (isEqual(command, "reset"))
  {
    configController->config.keypad = configController->defaultConfig->keypad;
    Serial.println("RESET OK");
  }
  else if (isEqual(command, "save"))
  {
    configController->saveConfig();
    Serial.println("SAVE OK");
  }
  else if (isEqual(command, "get"))
  {
    Serial.println("GET rt=" + String(configController->config.keypad.rapidTrigger));
    Serial.println("GET crt=" + String(configController->config.keypad.continuousRapidTrigger));
    Serial.println("GET rts=" + String(configController->config.keypad.rapidTriggerSensitivity));
    Serial.println("GET lh=" + String(configController->config.keypad.lowerHysteresis));
    Serial.println("GET uh=" + String(configController->config.keypad.upperHysteresis));
    Serial.println("GET key1=" + String(configController->config.keypad.key1));
    Serial.println("GET key2=" + String(configController->config.keypad.key2));
#ifdef KEYS_3
    Serial.println("GET key3=" + String(configController->config.keypad.key3));
#endif
    Serial.println("GET k1rp=" + String(configController->config.calibration.key1RestPosition));
    Serial.println("GET k1dp=" + String(configController->config.calibration.key1DownPosition));
    Serial.println("GET k2rp=" + String(configController->config.calibration.key2RestPosition));
    Serial.println("GET k2dp=" + String(configController->config.calibration.key2DownPosition));
#ifdef KEYS_3
    Serial.println("GET k3rp=" + String(configController->config.calibration.key3RestPosition));
    Serial.println("GET k3dp=" + String(configController->config.calibration.key3DownPosition));
#endif
    Serial.println("GET hid1=" + String(configController->config.keypad.key1HIDEnabled));
    Serial.println("GET hid2=" + String(configController->config.keypad.key2HIDEnabled));
#ifdef KEYS_3
    Serial.println("GET hid3=" + String(configController->config.keypad.key3HIDEnabled));
#endif
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
  if(valueIntSigned < 0)
    return;
    
  // Turn the signed integer into an unsigned one that is then used for further code.
  uint16_t valueInt = valueIntSigned;

  // If the parsed integer is different from the value string, the parsing defaults to 0
  // TODO: Improve this comparison by comparing char* instead
  if (String(valueInt) != String(value))
  {
    // In that case, check if the string length is 1 meaning it was possibly a character entered
    // For simplicity of handling setting config values, the character is turned into an integer
    if (strlen(value) == 1)
      valueInt = (int16_t)value[0];
    // If the string could not be parsed into an integer and is not 1 character long, ignore the command
    else
      return;
  }

  // rapid trigger
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

  // continuous rapid trigger
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

  // rapid trigger sensitivity
  else if (isEqual(key, "rts"))
  {
    // Check if the value is between 0 and 400.
    // TODO: implement tolerances
    if (valueInt < 0 || valueInt > 400)
    {
      Serial.println("Invalid value for 'rapidTriggerSensitivity'. Expected: 0-400, Actual: " + String(valueInt));
      return;
    }

    // Set the rapid sensitivity setting to the integer entered.
    configController->config.keypad.rapidTriggerSensitivity = valueInt;
    Serial.println("'rapidTriggerSensitivity' was set to '" + String(valueInt) + "'");
  }

  // lower hysteresis
  else if (isEqual(key, "lh"))
  {
    // Check if the value is bigger or equal to 0 and smaller than the upper hysteresis.
    // TODO: implement tolerances
    if (valueInt < 0 || valueInt >= configController->config.keypad.upperHysteresis)
    {
      Serial.println("Invalid value for 'lowerHysteresis'. Expected: 0-" + String(configController->config.keypad.upperHysteresis - 1) + ", Actual: " + String(valueInt));
      return;
    }

    // Set the lower hysteresis setting to the integer entered.
    configController->config.keypad.lowerHysteresis = valueInt;
    Serial.println("'lowerHysteresis' was set to '" + String(valueInt) + "'");
  }

  // upper hysteresis
  else if (isEqual(key, "uh"))
  {
    // Check if the value is bigger than the lower hysteresis and smaller or equal to 400.
    // TODO: implement tolerances
    if (valueInt <= configController->config.keypad.lowerHysteresis || valueInt > 400)
    {
      Serial.println("Invalid value for 'upperHysteresis'. Expected: " + String(configController->config.keypad.lowerHysteresis + 1) + "-400, Actual: " + String(valueInt));
      return;
    }

    // Set the upper hysteresis setting to the integer entered.
    configController->config.keypad.upperHysteresis = valueInt;
    Serial.println("'upperHysteresis' was set to '" + String(valueInt) + "'");
  }

  // keys
#ifdef KEYS_3
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
    if (isEqual(key, "key1"))
      configController->config.keypad.key1 = valueInt;
#ifdef KEYS_3
    else if(isEqual(key, "key3"))
      configController->config.keypad.key3 = valueInt;
#endif
    else
      configController->config.keypad.key2 = valueInt;

    Serial.println("'" + String(valueInt) + "' was set to '" + String(valueInt) + "'");
  }

  // rest position for the keys
#ifdef KEYS_3
  else if (isEqual(key, "k1rp") || isEqual(key, "k2rp") || isEqual(key, "k3rp"))
#else
  else if (isEqual(key, "k1rp") || isEqual(key, "k2rp"))
#endif
  {
    // Get the corresponding down position of the target key to perform range comparisons
    uint16_t downPosition = configController->config.calibration.key1DownPosition;
    if (isEqual(key, "k2rp"))
      downPosition = configController->config.calibration.key2DownPosition;
#ifdef KEYS_3
    else if(isEqual(key, "k3rp"))
      downPosition = configController->config.calibration.key3DownPosition;
#endif

    // Check if the value is bigger than the down position and smaller or equal to 1023.
    if (valueInt <= downPosition || valueInt > 1023)
    {
      Serial.println("Invalid value for '" + String(key) + "'. Expected: " + String(downPosition + 1) + "-1023, Actual: " + String(valueInt));
      return;
    }

    // Set the key rest position setting to the integer entered.
    if (isEqual(key, "k1rp"))
      configController->config.calibration.key1RestPosition = valueInt;
#ifdef KEYS_3
    else if(isEqual(key, "k3rp"))
      configController->config.calibration.key3RestPosition = valueInt;
#endif
    else
      configController->config.calibration.key2RestPosition = valueInt;

    Serial.println("'" + String(key) + "' was set to '" + String(valueInt) + "'");
  }

  // down position for the keys
#ifdef KEYS_3
  else if (isEqual(key, "k1dp") || isEqual(key, "k2dp") || isEqual(key, "k3dp"))
#else
  else if (isEqual(key, "k1dp") || isEqual(key, "k2dp"))
#endif
  {
    // Get the corresponding rest position of the target key to perform range comparisons
    uint16_t restPosition = configController->config.calibration.key1RestPosition;
    if (isEqual(key, "k2dp"))
      restPosition = configController->config.calibration.key2RestPosition;
#ifdef KEYS_3
    else if(isEqual(key, "k3dp"))
      restPosition = configController->config.calibration.key3DownPosition;
#endif

    // Check if the value is bigger or equal to 0 and smaller than the rest positon.
    if (valueInt < 0 || valueInt >= restPosition)
    {
      Serial.println("Invalid value for '" + String(key) + "'. Expected: 0-" + String(restPosition - 1) + ", Actual: " + String(valueInt));
      return;
    }

    // Set the key down position setting to the integer entered.
    if (isEqual(key, "k1dp"))
      configController->config.calibration.key1DownPosition = valueInt;
#ifdef KEYS_3
    else if(isEqual(key, "k3dp"))
      configController->config.calibration.key3DownPosition = valueInt;
#endif
    else
      configController->config.calibration.key2DownPosition = valueInt;

    Serial.println("'" + String(key) + "' was set to '" + String(valueInt) + "'");
  }

  // hid state for the keys
#ifdef KEYS_3
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
    if (isEqual(key, "hid1"))
      configController->config.keypad.key1HIDEnabled = valueInt == 1;
#ifdef KEYS_3
    else if(isEqual(key, "hid3"))
      configController->config.keypad.key3HIDEnabled = valueInt == 1;
#endif
    else
      configController->config.keypad.key2HIDEnabled = valueInt == 1;

    Serial.println("'" + String(key) + "' was set to '" + String(valueInt == 1 ? "true" : "false") + "'");
  }
}

bool SerialHandler::isEqual(char *str1, const char *str2)
{
  return strcmp(str1, str2) == 0;
}