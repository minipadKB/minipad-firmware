// Import all needed libraries and headers
#include <Arduino.h>
#include <Keyboard.h>
#include "main.hpp"
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"
#include "definitions.hpp"

// The version of this firmware in the YYYYMMDD.PATCH format (e.g. 20221219.2 for the 2nd release on the 19th december 2022)
#ifdef KEYS_3
char *firmwareVersion = (char *)"20221219.2-3k";
#else
char *firmwareVersion = (char *)"20221219.2-2k";
#endif

// Default configuration file loaded into the EEPROM if no configuration was set yet. Also used later on to reset the keypad and calibration
// structs that might get modified on a firmware update and have to be reset back to their default values then
Configuration defaultConfig =
{
  .version = Configuration::getVersion(),
  .name = {'m', 'i', 'n', 'i', 'p', 'a', 'd' },
  .keypad =
  {
    .version = KeypadConfiguration::getVersion(),
    .rapidTrigger = false,
    .continuousRapidTrigger = false,
    .rapidTriggerSensitivity = 30,
    .lowerHysteresis = 300,
    .upperHysteresis = 330,
#ifdef KEYS_3
    .key1 = 'z',
    .key2 = 'x',
    .key3 = 'c',
#else
    .key1 = 'z',
    .key2 = 'x',
#endif
    .key1HIDEnabled = true,
    .key2HIDEnabled = true,
#ifdef KEYS_3
    .key3HIDEnabled = true
#endif
  },
  .calibration =
  {
    .version = CalibrationConfiguration::getVersion(),
    .key1RestPosition = 450, .key1DownPosition = 150,
    .key2RestPosition = 450, .key2DownPosition = 150,
#ifdef KEYS_3
    .key3RestPosition = 450, .key3DownPosition = 150
#endif
  }
};

//
//                                             WARNING                                            
//
//         DO NOT CHANGE UNLESS ADVICED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR
//    THAT VIOLATES OSU!'S RULES. WE ARE NOT RESPONSIBLE FOR ANY RESTRICTIONS CAUSED FROM MODIFICATION.
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
uint16_t lastRapidTriggerValueKey1 = 0;
uint16_t lastRapidTriggerValueKey2 = 0;
#ifdef KEYS_3
uint16_t lastRapidTriggerValueKey3 = 0;
#endif

// Remember the state of all buttons to not send irrelevant HID commands
bool key1Pressed = false;
bool key2Pressed = false;
#ifdef KEYS_3
bool key3Pressed = false;
#endif

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
  uint16_t start = micros();
  // Check for any serial commands received for configuration
  while (Serial.available())
  {
    String str = Serial.readStringUntil('\n');
    serialHandler.handleSerialInput(&str);
  }

  // Read the hall effect sensors
  uint16_t value1 = analogRead(HE_PIN_1);
  uint16_t value2 = analogRead(HE_PIN_2);
#ifdef KEYS_3
  uint16_t value3 = analogRead(HE_PIN_3);
#endif

  /*Serial.println(value1);
  Serial.println(value2);
  Serial.println(value3);
  Serial.println("");*/

  // Map the values to the 0-400 range
  value1 = mapToRange400(value1, configController.config.calibration.key1DownPosition, configController.config.calibration.key1RestPosition);
  value2 = mapToRange400(value2, configController.config.calibration.key2DownPosition, configController.config.calibration.key2RestPosition);
#ifdef KEYS_3
  value3 = mapToRange400(value3, configController.config.calibration.key3DownPosition, configController.config.calibration.key3RestPosition);
#endif

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

#ifdef KEYS_3
    // Repeat the same for the third key
    if (value3 <= lastRapidTriggerValueKey3 - configController.config.keypad.rapidTriggerSensitivity && !key3Pressed)
      pressKey3();
    else if (value2 >= lastRapidTriggerValueKey3 + configController.config.keypad.rapidTriggerSensitivity && key3Pressed)
      releaseKey3();

    if ((key3Pressed && value3 < lastRapidTriggerValueKey3) || (!key3Pressed && value3 > lastRapidTriggerValueKey3))
      lastRapidTriggerValueKey3 = value3;
#endif
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
      
#ifdef KEYS_3
    // Handle actuation for key 3 by checking whether the value passes the lower or upper hysteresis
    if (value3 <= configController.config.keypad.lowerHysteresis && !key3Pressed)
      pressKey3();
    else if (value3 >= configController.config.keypad.upperHysteresis && key3Pressed)
      releaseKey3();
#endif
  }

  uint16_t end = micros();
  //Serial.println(String(end - start));
}

uint16_t mapToRange400(uint16_t value, uint16_t minimum, uint16_t maximum)
{
  return constrain(map(value, minimum, maximum, 0, 400), 0, 400);
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

#ifdef KEYS_3
void pressKey3()
{
  key3Pressed = true;
  if (configController.config.keypad.key3HIDEnabled)
    Keyboard.press(configController.config.keypad.key3);
}

void releaseKey3()
{
  if (configController.config.keypad.key3HIDEnabled)
    Keyboard.release(configController.config.keypad.key3);
  key3Pressed = false;
}
#endif