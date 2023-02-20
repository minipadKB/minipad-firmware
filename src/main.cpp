// Import all needed libraries and headers
#include <Arduino.h>
#include <EEPROM.h>
#include <Keyboard.h>
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "definitions.hpp"

// Default configuration file loaded into the EEPROM if no configuration was saved yet. Also used to reset the keypad and calibration
// structs that might get modified on a firmware update and have to be reset back to their default values then later on.
Configuration defaultConfig =
{
  .version = Configuration::getVersion(),
  .name = {'m', 'i', 'n', 'i', 'p', 'a', 'd' },
  .keypad =
  {
    .version = KeypadConfiguration::getVersion(),
    .rapidTrigger = false,
    .continuousRapidTrigger = false,
    .rapidTriggerUpSensitivity = 50,
    .rapidTriggerDownSensitivity = 50,
    .lowerHysteresis = 250,
    .upperHysteresis = 300,
#if KEYS == 3
    .keyChars = { 'z', 'x', 'c' },
    .hidEnabled = { true, true, true }
#else
    .keyChars = { 'z', 'x' },
    .hidEnabled = { true, true }
#endif
  },
  .calibration =
  {
    .version = CalibrationConfiguration::getVersion(),
#if KEYS == 3
    .restPositions = { 1800, 1800, 1800 },
    .downPositions = { 1100, 1100, 1100 }
#else
    .restPositions = { 1800, 1800 },
    .downPositions = { 1100, 1100 }
#endif
  }
};

ConfigurationController configController = ConfigurationController(&defaultConfig);
SerialHandler serialHandler = SerialHandler(&configController);
KeypadHandler keypadHandler = KeypadHandler(&configController);

void setup()
{
    // Initialize the EEPROM and load the configuration from it.
    EEPROM.begin(1024);
    configController.loadConfig();

    // Initialize the serial and HID interface.
    Serial.begin(115200);
    Keyboard.begin();

    // Set the amount of bits for the ADC to 12 for a better resolution on the analog readings.
    analogReadResolution(12);
}

void loop()
{
    // Check for any serial commands received.
    while (Serial.available())
    {
        String str = Serial.readStringUntil('\n');
        serialHandler.handleSerialInput(&str);
    }

    // Run the keypad handler checks to handle the actual keypad functionality.
    keypadHandler.check();
}