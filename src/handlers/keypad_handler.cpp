#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/keypad_handler.hpp"
#include "helpers/string_helper.hpp"
#include "definitions.hpp"

// Initialize the pins array once from the defined pins.
static const uint8_t pins[] = HE_PINS;

void KeypadHandler::check()
{
    // Go through all keys and run the checks.
    for (Key key : ConfigController.config.keys)
    {
        // Read the value from the hall effect sensor and map it to the 400 range.
        uint16_t rawValue = read(key);
        uint16_t mappedValue = mapTo400Range(key, rawValue);

        // Run either the rapid trigger or the traditional mode checks.
        if (key.rapidTrigger)
            checkRapidTrigger(key, mappedValue);
        else
            checkTraditional(key, mappedValue);
    }

    // Send the key report via the HID interface after updating the report.
    Keyboard.sendReport();
}

void KeypadHandler::checkTraditional(Key key, uint16_t value)
{
    // Check whether the value passes the lower or upper hysteresis.
    // If the value drops <= the lower hysteresis, the key is pressed down.
    // If the value rises >= the upper hysteresis, the key is released.
    if (value <= key.lowerHysteresis)
        pressKey(key);
    else if (value >= key.upperHysteresis)
        releaseKey(key);
}

void KeypadHandler::checkRapidTrigger(Key key, uint16_t value)
{
    // If the value is above the upper hysteresis the value is not (anymore) inside the rapid trigger zone
    // meaning the rapid trigger state for the key has to be set to false in order to be processed by further checks.
    // This only applies if continuous rapid trigger is not enabled as it only resets the state when the key is fully released.
    if (value >= key.upperHysteresis && !key.continuousRapidTrigger)
        _rapidTriggerEnabled[key.index] = false;
    // If continuous rapid trigger is enabled, the state is only reset to false when the key is fully released (<0.1mm aka 390+).
    else if (value >= 390 && key.continuousRapidTrigger)
        _rapidTriggerEnabled[key.index] = false;

    // If the value is below the lower hysteresis and the rapid trigger state is false on the key, press the key because the action of entering
    // the rapid trigger zone is already counted as a trigger. From there on, the actuation point moves dynamically in that zone.
    // Also the rapid trigger state for the key has to be set to true in order to be processed by furture loops.
    if (value <= key.lowerHysteresis && !_rapidTriggerEnabled[key.index])
    {
        pressKey(key);
        _rapidTriggerEnabled[key.index] = true;
    }
    // Check whether the key is not pressed but should be pressed.
    else if (!_keyPressedStates[key.index] && checkRapidTriggerPressKey(key, value))
        pressKey(key);
    // Check whether the key is pressed and should be released.
    else if (_keyPressedStates[key.index] && checkRapidTriggerReleaseKey(key, value))
        releaseKey(key);

    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((_keyPressedStates[key.index] && value < _currentRapidTriggerPeak[key.index]) || (!_keyPressedStates[key.index] && value > _currentRapidTriggerPeak[key.index]))
        _currentRapidTriggerPeak[key.index] = value;
}

bool KeypadHandler::checkRapidTriggerPressKey(Key key, uint16_t value)
{
    // Do not press the key if rapid trigger is not enabled. (aka. the value is not inside the actuation range)
    if (!_rapidTriggerEnabled[key.index])
        return false;

    // Check whether the read value drops more than (down sensitivity) below the highest recorded value.
    // If it is, press the key since the required down movement was done.
    // The int16_t conversions are done to prevent an integer underflow on the substraction if the rapid trigger peak is 0.
    if ((int16_t)value <= (int16_t)_currentRapidTriggerPeak[key.index] - (int16_t)key.rapidTriggerDownSensitivity)
        return true;

    return false;
}

bool KeypadHandler::checkRapidTriggerReleaseKey(Key key, uint16_t value)
{
    // Always release the key if the rapid trigger state on the key is false meaning
    // the value is outside the actuation range. (or <0.1mm on continuous rapid trigger)
    if (!_rapidTriggerEnabled[key.index])
        return true;

    // Check whether the read value is (up sensitivity) above the lowest recorded value.
    // If it is, press the key since the required down movement was done.
    if (value >= _currentRapidTriggerPeak[key.index] + key.rapidTriggerUpSensitivity)
        return true;

    return false;
}

void KeypadHandler::pressKey(Key key)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (_keyPressedStates[key.index] || !key.hidEnabled)
        return;

    // Send the HID instruction to the computer.
    _keyPressedStates[key.index] = true;
    Keyboard.press(key.keyChar);
}

void KeypadHandler::releaseKey(Key key)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!_keyPressedStates[key.index])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(key.keyChar);
    _keyPressedStates[key.index] = false;
}

uint16_t KeypadHandler::read(Key key)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[key.index]);

    // Average it out with the last 15 values in order to get a more stable value.
    // First, set the next value in the circular buffer to the read value.
    _lastValues[key.index][_nextLastValuesIndex[key.index]] = value;

    // Move the index by 1 and reset it back to 0 if it reached the end of the buffer.
    _nextLastValuesIndex[key.index] = (_nextLastValuesIndex[key.index] + 1) % 16;

    // Get the sum of all values in the buffer.
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++)
        sum += _lastValues[key.index][i];

    // Divide the number by 16 using bit operations as it is significantly faster than division.
    value = sum >> 4;

    return value;
}

uint16_t KeypadHandler::mapTo400Range(Key key, uint16_t value)
{
    // Map the value with the calibrated down and rest position values to a range between 0 and 400.
    int16_t mapped = map(value, key.downPosition, key.restPosition, 0, 400);

    // Then constrain it to a value between 0 and 400.
    return constrain(mapped, 0, 400);
}