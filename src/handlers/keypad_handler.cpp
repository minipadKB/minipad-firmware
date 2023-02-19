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
    for (uint8_t keyIndex = 0; keyIndex < KEYS; keyIndex++)
    {
        // Read the processed value from the hall effect sensor.
        uint16_t value = read(keyIndex);

        // Run either the rapid trigger or the traditional mode checks.
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
    if ((int16_t)value <= (int16_t)currentRapidTriggerPeak[keyIndex] - (int16_t)configController->config.keypad.rapidTriggerDownSensitivity)
        return true;

    return false;
}

bool KeypadHandler::checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value)
{
    // Check whether the read value rises more than (up sensitivity) above the lowest recorded value.
    // This represents the dynamic actuation point my moving the upper hysteresis while the button moves down.
    if (value >= currentRapidTriggerPeak[keyIndex] + configController->config.keypad.rapidTriggerUpSensitivity)
        return true;

    return false;
}

void KeypadHandler::pressKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (keyPressedStates[keyIndex] || !configController->config.keypad.hidEnabled[keyIndex])
        return;

    // Send the HID instruction to the computer.
    keyPressedStates[keyIndex] = true;
    Keyboard.press(configController->config.keypad.keyChars[keyIndex]);
}

void KeypadHandler::releaseKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!keyPressedStates[keyIndex])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(configController->config.keypad.keyChars[keyIndex]);
    keyPressedStates[keyIndex] = false;
}

uint16_t KeypadHandler::read(uint8_t keyIndex)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[keyIndex]);

    // Calculate the total delta (difference between rest position and down position) and the delta of the read value from the down position.
    // The int16_t conversions are done to prevent an integer underflow on the substraction if the read value is smaller than the down position.
    uint16_t totalDelta = configController->config.calibration.restPositions[keyIndex] - configController->config.calibration.downPositions[keyIndex];
    uint16_t delta = constrain((int16_t)value - (int16_t)configController->config.calibration.downPositions[keyIndex], 0, totalDelta);

    // Square the two deltas to map it accordingly afterwards. This turns out to be needed due to the behavior of magnetic field strength <-> distance.
    // The uint32_t conversions are done to prevent an overflow as the result of uint16 * uint16 is a uint16, but the maximum (4095 * 4095) exceeds it's limit.
    uint32_t totalDeltaSquared = totalDelta * (uint32_t)totalDelta;
    uint32_t deltaSquared = delta * (uint32_t)delta;

    // Map the squared delta from the range 0 to the total delta squared to 0-400 to get a linear scale and return it.
    return map(deltaSquared, 0, totalDeltaSquared, 0, 400);
}