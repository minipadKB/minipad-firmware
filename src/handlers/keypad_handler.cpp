#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/keypad_handler.hpp"
#include "helpers/string_helper.hpp"
#include "definitions.hpp"

void KeypadHandler::handle()
{
    // Go through all keys and run the checks.
    for (Key key : ConfigController.config.keys)
    {
        // Read the value from the hall effect sensor and map it to the travel distance range.
        uint16_t rawValue = read(key);
        uint16_t mappedValue = mapToTravelDistance(key, rawValue);

        // If the output mode is enabled, output the raw and mapped values.
        if (outputMode)
            Serial.printf("key%d=%d %d\n", key.index + 1, rawValue, mappedValue);

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
        _keyStates[key.index].rapidTriggerEnabled = false;
    // If continuous rapid trigger is enabled, the state is only reset to false when the key is fully released (<0.1mm aka 390+).
    else if (value >= 390 && key.continuousRapidTrigger)
        _keyStates[key.index].rapidTriggerEnabled = false;

    // If the value is below the lower hysteresis and the rapid trigger state is false on the key, press the key because the action of entering
    // the rapid trigger zone is already counted as a trigger. From there on, the actuation point moves dynamically in that zone.
    // Also the rapid trigger state for the key has to be set to true in order to be processed by furture loops.
    if (value <= key.lowerHysteresis && !_keyStates[key.index].rapidTriggerEnabled)
    {
        pressKey(key);
        _keyStates[key.index].rapidTriggerEnabled = true;
    }
    // Check whether the key is not pressed but should be pressed.
    else if (!_keyStates[key.index].pressed && checkRapidTriggerPressKey(key, value))
        pressKey(key);
    // Check whether the key is pressed and should be released.
    else if (_keyStates[key.index].pressed && checkRapidTriggerReleaseKey(key, value))
        releaseKey(key);

    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((_keyStates[key.index].pressed && value < _keyStates[key.index].rapidTriggerPeak) || (!_keyStates[key.index].pressed && value > _keyStates[key.index].rapidTriggerPeak))
        _keyStates[key.index].rapidTriggerPeak = value;
}

bool KeypadHandler::checkRapidTriggerPressKey(Key key, uint16_t value)
{
    // Do not press the key if rapid trigger is not enabled. (aka. the value is not inside the actuation range)
    if (!_keyStates[key.index].rapidTriggerEnabled)
        return false;

    // Check whether the read value drops more than (down sensitivity) below the highest recorded value.
    // If it is, press the key since the required down movement was done.
    // The int16_t conversions are done to prevent an integer underflow on the substraction if the rapid trigger peak is 0.
    if ((int16_t)value <= (int16_t)_keyStates[key.index].rapidTriggerPeak - (int16_t)key.rapidTriggerDownSensitivity)
        return true;

    return false;
}

bool KeypadHandler::checkRapidTriggerReleaseKey(Key key, uint16_t value)
{
    // Always release the key if the rapid trigger state on the key is false meaning
    // the value is outside the actuation range. (or <0.1mm on continuous rapid trigger)
    if (!_keyStates[key.index].rapidTriggerEnabled)
        return true;

    // Check whether the read value is (up sensitivity) above the lowest recorded value.
    // If it is, press the key since the required down movement was done.
    if (value >= _keyStates[key.index].rapidTriggerPeak + key.rapidTriggerUpSensitivity)
        return true;

    return false;
}

void KeypadHandler::pressKey(Key key)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (_keyStates[key.index].pressed || !key.hidEnabled)
        return;

    // Send the HID instruction to the computer.
    _keyStates[key.index].pressed = true;
    Keyboard.press(key.keyChar);
}

void KeypadHandler::releaseKey(Key key)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!_keyStates[key.index].pressed)
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(key.keyChar);
    _keyStates[key.index].pressed = false;
}

// Initialize the pins array once from the defined pins.
static const uint8_t pins[] = HE_PINS;

uint16_t KeypadHandler::read(Key key)
{
    // Read the value from the port of the specified key, filter it through the SMA filter and return it.
    return _keyStates[key.index].filter(analogRead(pins[key.index]));
}

uint16_t KeypadHandler::mapToTravelDistance(Key key, uint16_t value)
{
    // Map the value with the calibrated down and rest position values to a range between 0 and TRAVEL_DISTANCE_IN_0_01MM and constrain it.
    // This is done to guarantee that the unit for the numbers used across the firmware actually matches the milimeter metric.
    return constrain(map(value, key.downPosition, key.restPosition, 0, TRAVEL_DISTANCE_IN_0_01MM), 0, TRAVEL_DISTANCE_IN_0_01MM);
}