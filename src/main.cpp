// Import all needed libraries and headers
#include <Arduino.h>
#include <Keyboard.h>
#include "main.hpp"
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"

// Define the ports used for the HE sensors
#define HE_PIN_1 A1
#define HE_PIN_2 A2

// Safemode boundary that is checked for upon firmware bootup. If both pins read a value <= this value,
// a safemode is activated causing the config to temporarily switch to the default config
#define SAFEMODE_BOUNDARY 20

// Minimum sensitivity for rapid trigger, minimum amount the hysteresis have to be below the rest position and
// the minimum difference between the lower and upper hysteresis. Used to ensure that fluctuation will not accidentally trigger the key
#define TOLERANCE 10

// The version of this firmware in the YYYYMMDD.PATCH format (e.g. 20221219.2 for the 2nd release on the 19th december 2022)
char *firmwareVersion = (char *)"20221219.2";

// Default configuration file loaded into the EEPROM if no configuration was set yet. Also used later on to reset the keypad and calibration
// structs that might get modified on a firmware update and have to be reset back to their default values then
Configuration defaultConfig =
{
  .version = Configuration::getVersion(),
  .name = {'t', 'e', 's', 't'},
  .keypad =
  {
    .version = KeypadConfiguration::getVersion(),
    .rapidTrigger = false,
    .continuousRapidTrigger = false,
    .rapidTriggerSensitivity = 10,
    .lowerHysteresis = 300,
    .upperHysteresis = 330,
    .key1 = 'x',
    .key2 = 'z',
    .key1HIDEnabled = true,
    .key2HIDEnabled = true
  },
  .calibration =
  {
    .version = CalibrationConfiguration::getVersion(),
    .key1RestPosition = 450, 
    .key2RestPosition = 450, 
    .key1DownPosition = 150, .key2DownPosition = 150
  }
};

//
//                                            WARNING                                            
//
//        DO NOT CHANGE UNLESS ADVICED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR
//    THAT VIOLATES OSU'S RULES. WE ARE NOT RESPONSIBLE FOR ANY RESTRICTIONS CAUSED FROM MODIFICATION.
//
ToleranceConfiguration tolerances =
{
  .hysteresisTolerance = 10,
  .rapidTriggerTolerance = 15
};

ConfigurationController configController = ConfigurationController(&defaultConfig);

SerialHandler serialHandler = SerialHandler(&configController, firmwareVersion);

// Integers to remember the lowest/highest value the analog value gets to if rapid trigger is enabled.
// If the button is pressed down, it saves the lowest value it got to. If the value read is higher
// than this value by the rapid trigger sensitivity, the button is released. The other way around,
// if the button is not pressed down, it saves the highest value it got to. If the value read is lower
// than this value by the rapid trigger sensitivity, the button is pressed.
int lastRapidTriggerValueKey1 = 0;
int lastRapidTriggerValueKey2 = 0;

// Remember the state of both buttons to not send irrelevant HID commands
bool key1Pressed = false;
bool key2Pressed = false;

void setup()
{
  Serial.begin(115200);

  // Load the configuration
  configController.loadConfig();

  // Initialize the serial and HID interface
  Keyboard.begin();
}

void loop()
{
  // Check for any serial commands received for configuration
  while (Serial.available())
    serialHandler.handleSerialInput(&Serial.readStringUntil('\n'));

  // Read the hall effect sensors
  int value1 = analogRead(HE_PIN_1);
  int value2 = analogRead(HE_PIN_2);

  // Map the values to the 0-400 range
  value1 = mapToRange400(value1, configController.config.calibration.key1DownPosition, configController.config.calibration.key1RestPosition);
  value2 = mapToRange400(value2, configController.config.calibration.key2DownPosition, configController.config.calibration.key2RestPosition);

  if (configController.config.keypad.rapidTrigger)
  {
    // Check whether the read value for key 1 dropped below/rises above the actuation point depending on the current button state.
    // If the button is not pressed and the value drops more than (sensitivity) below the highest recorded value, press the key
    // Furthermore, check if the value is at least 10 below the rest position such that the user won't accidentally
    // get stuck with a pressed down key while the key is all the way up.
    if (value1 <= lastRapidTriggerValueKey1 - configController.config.keypad.rapidTriggerSensitivity && !key1Pressed)
      pressKey1();
    // If the button is pressed and the value rises more than (sensitivity) above the highest recorded value, release the key
    else if (value1 >= lastRapidTriggerValueKey1 + configController.config.keypad.rapidTriggerSensitivity && key1Pressed)
      releaseKey1();

    // If the key is pressed, save the lowest value; If the key is not pressed, save the highest value
    if ((key1Pressed && value1 < lastRapidTriggerValueKey1) || (!key1Pressed && value1 > lastRapidTriggerValueKey1))
      lastRapidTriggerValueKey1 = value1;

    // Repeat the same for the second key
    if (value2 <= lastRapidTriggerValueKey2 - configController.config.keypad.rapidTriggerSensitivity && !key2Pressed)
      pressKey2();
    else if (value2 >= lastRapidTriggerValueKey2 + configController.config.keypad.rapidTriggerSensitivity && key2Pressed)
      releaseKey2();

    if ((key2Pressed && value2 < lastRapidTriggerValueKey2) || (!key2Pressed && value2 > lastRapidTriggerValueKey2))
      lastRapidTriggerValueKey2 = value2;
  }
  else
  {
    // Handle actuation for key 1 by checking whether the value passes the lower or upper hysteresis
    if (value1 <= configController.config.keypad.lowerHysteresis && !key1Pressed)
      pressKey1();
    else if (value1 >= configController.config.keypad.upperHysteresis && key1Pressed)
      releaseKey1();

    // Handle actuation for key 2 by checking whether the value passes the lower or upper hysteresis
    if (value2 <= configController.config.keypad.lowerHysteresis && !key2Pressed)
      pressKey2();
    else if (value2 >= configController.config.keypad.upperHysteresis && key2Pressed)
      releaseKey2();
  }
}


uint16_t mapToRange400(uint16_t value, uint16_t min, uint16_t max)
{
  int16_t newValue = (long(value - min) * 400 / (max - min);
  return constrain(newValue, 0, 400);
}


void pressKey1()
{
  key1Pressed = true;
  if (configController.config.keypad.key1HIDEnabled)
    Keyboard.press(configController.config.keypad.key1);
}

void releaseKey1()
{
  if (configController.config.keypad.key1HIDEnabled)
    Keyboard.release(configController.config.keypad.key1);
  key1Pressed = false;
}

void pressKey2()
{
  key2Pressed = true;
  if (configController.config.keypad.key2HIDEnabled)
    Keyboard.press(configController.config.keypad.key2);
}

void releaseKey2()
{
  if (configController.config.keypad.key2HIDEnabled)
    Keyboard.release(configController.config.keypad.key2);
  key2Pressed = false;
}
