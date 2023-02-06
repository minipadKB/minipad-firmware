#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/keypad_handler.hpp"
#include "utils.hpp"
#include "definitions.hpp"

// Initialize the pins array once from the defined pins.
static const uint8_t pins[] = HE_PINS;

KeypadHandler::KeypadHandler(ConfigurationController *configController, ToleranceConfiguration *tolerances) : configController(configController), tolerances(tolerances) {}
KeypadHandler::~KeypadHandler() {}

void KeypadHandler::check()
{
#ifdef KEYS_3
    for (uint8_t keyIndex = 0; keyIndex < 3; keyIndex++)
#else
    for (uint8_t keyIndex = 0; keyIndex < 2; keyIndex++)
#endif
    {
        // Run either the rapid trigger or the traditional mode checks
        if (configController->config.keypad.rapidTrigger)
            checkRapidTriggerMode(keyIndex);
        else
            checkTraditionalMode(keyIndex);
    }
}

void KeypadHandler::checkTraditionalMode(uint8_t keyIndex)
{
    // Read the processed value from the hall effect sensor.
    uint16_t value = read(keyIndex);

    // Check whether the value passes the lower or upper hysteresis.
    // If the value drops <= the lower hysteresis, the key is pressed down.
    // IF the value rises >= the upper hysteresis, the key is released.
    if (value <= configController->config.keypad.lowerHysteresis)
        pressKey(keyIndex);
    else if (value >= configController->config.keypad.upperHysteresis)
        releaseKey(keyIndex);
}

void KeypadHandler::checkRapidTriggerMode(uint8_t keyIndex)
{
    // TBI
}

void KeypadHandler::pressKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if(pressedStates[keyIndex] || !configController->config.keypad.hidEnabled[keyIndex])
        return;

    // Send the HID instruction to the computer.
    pressedStates[keyIndex] = true;
    Keyboard.press(configController->config.keypad.keyChars[keyIndex]);
}

void KeypadHandler::releaseKey(uint8_t keyIndex)
{
    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if(!pressedStates[keyIndex])
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(configController->config.keypad.keyChars[keyIndex]);
    pressedStates[keyIndex] = false;
}

uint16_t KeypadHandler::read(uint8_t keyIndex)
{
    // Read the value from the port of the specified key.
    uint16_t value = analogRead(pins[keyIndex]);

    // Map the specified value with the specified minimum and maximum to the new value.
    // Then constrain it to a value between 0 and 400.
    int16_t mapped = map(value, configController->config.calibration.downPositions[keyIndex], configController->config.calibration.restPositions[keyIndex], 0, 400);
    return constrain(mapped, 0, 400);
}