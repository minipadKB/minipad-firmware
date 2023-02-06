#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/keypad_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

// Initialize the pins array once from the defined pins.
static const uint8_t pins[] = HE_PINS;

KeypadHandler::KeypadHandler(ConfigurationController *configController) : configController(configController) {}

KeypadHandler::~KeypadHandler() {}

void KeypadHandler::check()
{
    // Go through all keys and run the checks.
#ifdef KEYS_3
    for (uint8_t keyIndex = 0; keyIndex < 3; keyIndex++)
#else
    for (uint8_t keyIndex = 0; keyIndex < 2; keyIndex++)
#endif
    {
        // Read the processed value from the hall effect sensor.
        uint16_t value = read(keyIndex);

        // Run either the rapid trigger or the traditional mode checks
        if (configController->config.keypad.rapidTrigger)
            checkRapidTrigger(keyIndex, value);
        else
            checkTraditional(keyIndex, value);
    }
}

void KeypadHandler::checkTraditional(uint8_t keyIndex, uint16_t value)
{
    // Check whether the value passes the lower or upper hysteresis.
    // If the value drops <= the lower hysteresis, the key is pressed down.
    // IF the value rises >= the upper hysteresis, the key is released.
    if (value <= configController->config.keypad.lowerHysteresis)
        pressKey(keyIndex);
    else if (value >= configController->config.keypad.upperHysteresis)
        releaseKey(keyIndex);
}

void KeypadHandler::checkRapidTrigger(uint8_t keyIndex, uint16_t value)
{
    if (!pressedStates[keyIndex])
    {
        // If the key is not pressed, check whether the read value drops more than (down sensitivity) below the highest recorded value.
        // This represents the dynamic actuation point my moving the lower hysteresis while the button moves up.
        // The int16_t conversions are done to prevent an integer underflow on the substraction if the rapid trigger peak is 0.
        // Also make sure that this press is only triggered if the key is actuated, meaning the value is below the lower hysteresis.
        if ((int16_t)value <= (int16_t)currentRapidTriggerPeak[keyIndex] - (int16_t)configController->config.keypad.rapidTriggerDownSensitivity && value <= configController->config.keypad.lowerHysteresis)
            pressKey(keyIndex);

        // If the key is at an all-time high, save the value.
        if (value > currentRapidTriggerPeak[keyIndex])
            currentRapidTriggerPeak[keyIndex] = value;
    }
    else
    {
        // If the key is pressed, check whether the read value rises more than (up sensitivity) above the lowest recorded value.
        // This represents the dynamic actuation point my moving the upper hysteresis while the button moves down.
        // Otherwise, if the value is >= the upper hysteresis as well since rapid trigger is supposed to only operate when actuated.
        if (value >= (int16_t)currentRapidTriggerPeak[keyIndex] + configController->config.keypad.rapidTriggerUpSensitivity || value >= configController->config.keypad.upperHysteresis)
            releaseKey(keyIndex);

        // If the key is at an all-time low, save the value.
        if (value < currentRapidTriggerPeak[keyIndex])
            currentRapidTriggerPeak[keyIndex] = value;
    }
}

void KeypadHandler::pressKey(uint8_t keyIndex)
{
    Serial.println("press");
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (pressedStates[keyIndex] || !configController->config.keypad.hidEnabled[keyIndex])
        return;

    // Send the HID instruction to the computer.
    pressedStates[keyIndex] = true;
    Keyboard.press(configController->config.keypad.keyChars[keyIndex]);
}

void KeypadHandler::releaseKey(uint8_t keyIndex)
{
    Serial.println("release");
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!pressedStates[keyIndex])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(configController->config.keypad.keyChars[keyIndex]);
    pressedStates[keyIndex] = false;
}

uint16_t KeypadHandler::read(uint8_t keyIndex)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[keyIndex]);

    // Map the read value with the calibrated down and rest position values to a range between 0 and 400.
    int16_t mapped = map(value, configController->config.calibration.downPositions[keyIndex], configController->config.calibration.restPositions[keyIndex], 0, 400);

    // Then constrain it to a value between 0 and 400.
    return constrain(mapped, 0, 400);
}