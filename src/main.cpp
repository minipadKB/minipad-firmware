// Import all needed libraries and headers
#include <Arduino.h>
#include <Keyboard.h>
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "definitions.hpp"

// The version of this firmware in the YYYYMMDD.PATCH format. (e.g. 20221219.2 for the 2nd release on the 19th december 2022)
#ifdef KEYS_3
char *firmwareVersion = (char *)"20221219.2-3k";
#else
char *firmwareVersion = (char *)"20221219.2-2k";
#endif

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
    .lowerHysteresis = 300,
    .upperHysteresis = 330,
#ifdef KEYS_3
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
#ifdef KEYS_3
    .restPositions = { 450, 450, 450 },
    .downPositions = { 150, 150, 150 }
#else
    .restPositions = { 450, 450 },
    .downPositions = { 150, 150 }
#endif
  }
};

//
//                                           WARNING
//
//       DO NOT CHANGE UNLESS ADVICED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR
//    THAT VIOLATES OSU!'S RULES. CHANGING THIS MAY LEAD TO INCORRECT KEY PRESSES AND A RESTRICTION.
//
ToleranceConfiguration tolerances =
{
    .hysteresis = 10,
    .rapidTrigger = 15
};

ConfigurationController configController = ConfigurationController(&defaultConfig, &tolerances);
SerialHandler serialHandler = SerialHandler(&configController, firmwareVersion);
KeypadHandler keypadHandler = KeypadHandler(&configController);

void setup()
{
    // Load the configuration from the EEPROM.
    configController.loadConfig();

    // Initialize the serial and HID interface.
    Serial.begin(115200);
    Keyboard.begin();

    // Set the reference voltage for the ADC to 2.56V for a better resolution on the analog readings.
    analogReference(INTERNAL);
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