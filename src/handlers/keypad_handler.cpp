#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/keypad_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

// Initialize the pins array once from the defined pins.
static const uint8_t pins[] = HE_PINS;

void KeypadHandler::check()
{
    // Go through all keys and run the checks.
    for (uint8_t keyIndex = 0; keyIndex < KEYS; keyIndex++)
    {
        // Read the mapped value from the hall effect sensor.
        uint16_t value = read(keyIndex);

        // Send a serial message with the sensor value if in calibration mode.
        if (calibrationMode)
        {
            Serial.print("CAL key");
            Serial.print(keyIndex + 1);
            Serial.print("=");
            Serial.println(analogRead(pins[keyIndex]));
            Serial.print("CAL key");
            Serial.print(keyIndex + 1);
            Serial.print("mapped=");
            Serial.println(value);
        }
        // Run either the rapid trigger or the traditional mode checks.
        else if (ConfigController.config.keypad.rapidTrigger)
            checkRapidTrigger(keyIndex, value);
        else
            checkTraditional(keyIndex, value);
    }

    // Send the key report via the HID interface after updating the report.
    Keyboard.sendReport();
}

void KeypadHandler::checkTraditional(uint8_t keyIndex, uint16_t value)
{
    // Check whether the value passes the lower or upper hysteresis.
    // If the value drops <= the lower hysteresis, the key is pressed down.
    // IF the value rises >= the upper hysteresis, the key is released.
    if (value <= ConfigController.config.keypad.lowerHysteresis)
        pressKey(keyIndex);
    else if (value >= ConfigController.config.keypad.upperHysteresis)
        releaseKey(keyIndex);
}

void KeypadHandler::checkRapidTrigger(uint8_t keyIndex, uint16_t value)
{
    // If the value is below the lower hysteresis the value is inside the actuation range meaning
    // the rapid trigger state for the key has to be set to true in order to be processed by further checks.
    if (value <= ConfigController.config.keypad.lowerHysteresis)
        _rapidTriggerEnabled[keyIndex] = true;
    // If the value is above the upper hysteresis the value is not (anymore) inside the actuation range
    // meaning the rapid trigger state for the key has to be set to false in order to be processed by further checks.
    // This only applies if continuous rapid trigger is not enabled as it only resets the state when the key is fully released.
    else if (value >= ConfigController.config.keypad.upperHysteresis && !ConfigController.config.keypad.continuousRapidTrigger)
        _rapidTriggerEnabled[keyIndex] = false;
    // If continuous rapid trigger is enabled, the state is only reset to false when the key is fully released (<0.1mm aka 390+).
    else if (value >= 390 && ConfigController.config.keypad.continuousRapidTrigger)
        _rapidTriggerEnabled[keyIndex] = false;

    // Check whether the key is not pressed but should be pressed.
    if (!_keyPressedStates[keyIndex] && checkRapidTriggerPressKey(keyIndex, value))
        pressKey(keyIndex);
    // Check whether the key is pressed and should be released.
    else if (_keyPressedStates[keyIndex] && checkRapidTriggerReleaseKey(keyIndex, value))
        releaseKey(keyIndex);

    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((_keyPressedStates[keyIndex] && value < _currentRapidTriggerPeak[keyIndex]) || (!_keyPressedStates[keyIndex] && value > _currentRapidTriggerPeak[keyIndex]))
        _currentRapidTriggerPeak[keyIndex] = value;
}

bool KeypadHandler::checkRapidTriggerPressKey(uint8_t keyIndex, uint16_t value)
{
    // Do not press the key if rapid trigger is not enabled. (aka. the value is not inside the actuation range)
    if (!_rapidTriggerEnabled[keyIndex])
        return false;

    // Check whether the read value drops more than (down sensitivity) below the highest recorded value.
    // If it is, press the key since the required down movement was done.
    // The int16_t conversions are done to prevent an integer underflow on the substraction if the rapid trigger peak is 0.
    if ((int16_t)value <= (int16_t)_currentRapidTriggerPeak[keyIndex] - (int16_t)ConfigController.config.keypad.rapidTriggerDownSensitivity)
        return true;

    return false;
}

bool KeypadHandler::checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value)
{
    // Always release the key if the rapid trigger state on the key is false meaning
    // the value is outside the actuation range. (or <0.1mm on continuous rapid trigger)
    if (!_rapidTriggerEnabled[keyIndex])
        return true;

    // Check whether the read value is (up sensitivity) above the lowest recorded value.
    // If it is, press the key since the required down movement was done.
    if (value >= _currentRapidTriggerPeak[keyIndex] + ConfigController.config.keypad.rapidTriggerUpSensitivity)
        return true;

    return false;
}

void KeypadHandler::pressKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (_keyPressedStates[keyIndex] || !ConfigController.config.keypad.hidEnabled[keyIndex])
        return;

    // Send the HID instruction to the computer.
    _keyPressedStates[keyIndex] = true;
    Keyboard.press(ConfigController.config.keypad.keyChars[keyIndex]);
}

void KeypadHandler::releaseKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!_keyPressedStates[keyIndex])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(ConfigController.config.keypad.keyChars[keyIndex]);
    _keyPressedStates[keyIndex] = false;
}

uint16_t KeypadHandler::read(uint8_t keyIndex)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[keyIndex]);

    // Average it out with the last 31 values in order to get a more stable value.
    // First, set the next value in the circular buffer to the read value.
    _lastValues[keyIndex][_nextLastValuesIndex[keyIndex]] = value;

    // Move the index by 1 and reset it back to 0 if it reached the end of the buffer.
    _nextLastValuesIndex[keyIndex] = (_nextLastValuesIndex[keyIndex] + 1) % 32;

    Serial.println(value);
    // Get the sum of all values in the buffer.
    uint32_t sum = 0;
    for (int i = 0; i < 32; i++)
        sum += _lastValues[keyIndex][i];

    // Divide the number by 32 using bit operations as it is significantly faster than division.
    value = sum >> 5;

    // Map the read value with the calibrated down and rest position values to a range between 0 and 400.
    int16_t mapped = map(value, ConfigController.config.calibration.downPositions[keyIndex], ConfigController.config.calibration.restPositions[keyIndex], 0, 400);

    // Then constrain it to a value between 0 and 400.
    return constrain(mapped, 0, 400);
}