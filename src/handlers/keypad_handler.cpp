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
        // Read the processed value from the hall effect sensor.
        uint16_t value = read(keyIndex);

        // Run either the rapid trigger or the traditional mode checks.
        if (ConfigController.config.keypad.rapidTrigger)
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
    // Check whether the key is pressed and should be released.
    if (keyPressedStates[keyIndex] && checkRapidTriggerReleaseKey(keyIndex, value))
        releaseKey(keyIndex);
    // Check whether the key is not pressed but should be pressed.
    else if (!keyPressedStates[keyIndex] && checkRapidTriggerPressKey(keyIndex, value))
        pressKey(keyIndex);

    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((keyPressedStates[keyIndex] && value < currentRapidTriggerPeak[keyIndex]) || (!keyPressedStates[keyIndex] && value > currentRapidTriggerPeak[keyIndex]))
        currentRapidTriggerPeak[keyIndex] = value;
}

bool KeypadHandler::checkRapidTriggerPressKey(uint8_t keyIndex, uint16_t value)
{
    // Check whether the read value drops more than (down sensitivity) below the highest recorded value.
    // This represents the dynamic actuation point my moving the lower hysteresis while the button moves up.
    // The int16_t conversions are done to prevent an integer underflow on the substraction if the rapid trigger peak is 0.
    if ((int16_t)value <= (int16_t)currentRapidTriggerPeak[keyIndex] - (int16_t)ConfigController.config.keypad.rapidTriggerDownSensitivity)
        return true;

    return false;
}

bool KeypadHandler::checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value)
{
    // Check whether the read value rises more than (up sensitivity) above the lowest recorded value.
    // This represents the dynamic actuation point my moving the upper hysteresis while the button moves down.
    if (value >= currentRapidTriggerPeak[keyIndex] + ConfigController.config.keypad.rapidTriggerUpSensitivity)
        return true;

    return false;
}

void KeypadHandler::pressKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (keyPressedStates[keyIndex] || !ConfigController.config.keypad.hidEnabled[keyIndex])
        return;

    // Send the HID instruction to the computer.
    keyPressedStates[keyIndex] = true;
    Keyboard.press(ConfigController.config.keypad.keyChars[keyIndex]);
}

void KeypadHandler::releaseKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!keyPressedStates[keyIndex])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(ConfigController.config.keypad.keyChars[keyIndex]);
    keyPressedStates[keyIndex] = false;
}

uint16_t KeypadHandler::read(uint8_t keyIndex)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[keyIndex]);

    // Map the read value with the calibrated down and rest position values to a range between 0 and 400.
    int16_t mapped = map(value, ConfigController.config.calibration.downPositions[keyIndex], ConfigController.config.calibration.restPositions[keyIndex], 0, 400);

    // Then constrain it to a value between 0 and 400.
    return constrain(mapped, 0, 400);
}