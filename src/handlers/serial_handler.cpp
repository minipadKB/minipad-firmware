#include <Arduino.h>
#include "handlers/serial_handler.hpp"
#include "utils.hpp"

SerialHandler::SerialHandler(ConfigurationController *configController, char *firmwareVersion) : configController(configController), firmwareVersion(firmwareVersion) {}
SerialHandler::~SerialHandler() {}

void SerialHandler::handleSerialInput(String *inputStr)
{
  // Convert the string into a character array for further parsing
  char input[(*inputStr).length() + 1];
  (*inputStr).toCharArray(input, (*inputStr).length() + 1);

  // Get the first two arguments of the input
  char *arg0 = Utils::getArgumentAt(input, ' ', 0);
  char *arg1 = Utils::getArgumentAt(input, ' ', 1);

  // If no second argument has been specified, it's either a command or a get request
  if (isEqual(arg1, ""))
  {
    // Try to handle the input as a command and if successful, return
    if (handleCommand(arg0))
      return;

    // Otherwise handle it as a get request
    handleGet(arg0);
    return;
  }

  // If a second argument has been specified, it's a set request
  handleSet(arg0, arg1);
}

// Handles the input as a command and returns a bool whether the input was handled or not
bool SerialHandler::handleCommand(char *command)
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
  else
    return false;

  return true;
}

// Handles the input as a get request with the input being the key.
void SerialHandler::handleGet(char *key)
{
  if (isEqual(key, "get"))
  {
    Serial.println("GET rt=" + String(configController->config.keypad.rapidTrigger));
    Serial.println("GET rts=" + String(configController->config.keypad.rapidTriggerSensitivity));
    Serial.println("GET lh=" + String(configController->config.keypad.lowerHysteresis));
    Serial.println("GET uh=" + String(configController->config.keypad.upperHysteresis));
    Serial.println("GET key1=" + String(configController->config.keypad.key1));
    Serial.println("GET key2=" + String(configController->config.keypad.key2));
    Serial.println("GET k1rp=" + String(configController->config.calibration.key1RestPosition));
    Serial.println("GET k2rp=" + String(configController->config.calibration.key2RestPosition));
    Serial.println("GET k1dp=" + String(configController->config.calibration.key1DownPosition));
    Serial.println("GET k2dp=" + String(configController->config.calibration.key2DownPosition));
    Serial.println("GET hid1=" + String(configController->config.keypad.key1HIDEnabled));
    Serial.println("GET hid2=" + String(configController->config.keypad.key2HIDEnabled));
    Serial.println("GET name=" + String(configController->config.name));
    Serial.println("GET END");
  }
}

// Handles the input as a set request with the input being the key and the value.
void SerialHandler::handleSet(char *key, char *value)
{
}

bool SerialHandler::isEqual(char *str1, char *str2)
{
  return strcmp(str1, str2) == 0;
}