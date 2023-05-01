#include <Arduino.h>
#include <Keyboard.h>
#include "config/keys/key_type.hpp"
#include "handlers/keypad_handler.hpp"
#include "helpers/string_helper.hpp"
#include "definitions.hpp"

/*
   Explanation of the Rapid Trigger Logic

   Rapid Trigger has 3 different settings:
   - (bool) continuous rapid trigger
   - (number) up sensitivity
   - (number) down sensitivity

   Rapid Trigger also has these 3 states:
   - (bool) whether key is pressed down
   - (bool) whether the key is inside the rapid trigger zone
   - (number) the current peak of the travel distance

   General Rapid Trigger description:

   Rapid Trigger allows the key to be dynamically pressed or released, depending on the distance travelled instead of
   being at a fixed actuation point. This is done by always remembering the lowest/highest peak the value reached
   and using it for reference. For example, if I press the key down by 3.5mm with my up sensitivity set to 0.2mm, the
   firmware will remember the 3.5mm as the peak, allowing me to move freely within 3.5-3.3mm without any changes and
   only releasing the key if I release the key by more than 0.2mm starting from the peak, meaning 3.3mm. The same logic
   applies to pressing the key down. If I move the key further up to 2.0mm with a down sensitivity of 0.5mm, the firmware
   will remember the peak of 2.0mm and not do anything between 2.0-2.5mm, since if I press the key back in beyond 2.5mm
   it will start pressing it again. If I do not push it beyong 2.5mm and instead decide to go further up than 2.0mm,
   the peak will of course updates as expected. This is the main logic that applies inside the so-called Rapid Trigger zone.

   Rapid Trigger Zone:

   Rapid Trigger is only active inside the Rapid Trigger Zone. The Rapid Trigger Zone is the zone below the actuation point,
   down to all the way pressed in. If the key enters that zone, a keypress is also performed, just like you're used to the
   traditional mode of how keyboards work. As long as the key is inside that zone, the logic described above applies.
   If the key leaves that zone, the key is always being released and the Rapid Trigger logic no longer applies.
   This is where the feature continuous rapid trigger (CRT) comes into play. Continuous rapid trigger causes the firmware to
   apply the Rapid Trigger logic over almost the full actuation range, to be specific everything >0.1mm travel distance.
   The only condition is that the key already entered the Rapid Trigger zone once. If CRT is enabled, once the key has
   entered the Rapid Trigger zone the logic described in the first paragraph applies until the key is *fully released*, meaning
   pressed in <0.1mm.

   Workflow of the Rapid Trigger code:

   The Rapid Trigger implementation in this firmware consists of 4 steps.
   Step 1: Check whether the key left the Rapid Trigger zone (normal) or was fully released (CRT mode)
   Step 2: Check whether the key has entered the Rapid Trigger zone, updating the inRapidTriggerZone state and pressing the key
   Step 3: Apply the dynamic travel distance checks, the core of the Rapid Trigger feature
   Step 4: Depending on whether the key is pressed or not, remember the lowest/highest peak achieved
*/

void KeypadHandler::handle()
{
    // Go through all hall effect keys and run the checks.
    for (const HEKey &key : ConfigController.config.heKeys)
    {
        // Read the value from the hall effect sensor and map it to the travel distance range.
        uint16_t rawValue = readKey(key);
        uint16_t mappedValue = mapSensorValueToTravelDistance(key, rawValue);

        // If the output mode is enabled, output the raw and mapped values.
        if (outputMode)
            Serial.printf("OUT hkey%d=%d %d\n", key.index + 1, rawValue, mappedValue);

        // Run the checks on the HE key.
        checkHEKey(key, mappedValue);
    }

    // Go through all digital keys and run the checks.
    for (const DigitalKey &key : ConfigController.config.digitalKeys)
    {
        // Read the digital value from the key pin.
        bool pressed = readKey(key);

        // Run the checks on the digital key.
        checkDigitalKey(key, pressed);
    }

    // Send the key report via the HID interface after updating the report.
    Keyboard.sendReport();
}

void KeypadHandler::checkHEKey(const HEKey &key, uint16_t value)
{
    // If the key is in traditional mode, do the usual hysteresis checks.
    if (!key.rapidTrigger)
    {
        // Check whether the value passes the lower or upper hysteresis.
        // If the value drops <= the lower hysteresis, the key is pressed down.
        // If the value rises >= the upper hysteresis, the key is released.
        if (value <= key.lowerHysteresis)
            pressKey(key);
        else if (value >= key.upperHysteresis)
            releaseKey(key);

        // Return here to not run into the rapid trigger code.
        return;
    }

    // RT STEP 1: Reset the rapid trigger state if the value left the rapid trigger zone (normal) or was fully released (CRT).
    // If the value is above the upper hysteresis the value is not (anymore) inside the rapid trigger zone
    // meaning the rapid trigger state for the key has to be set to false in order to be processed by further checks.
    // This only applies if continuous rapid trigger is not enabled as it only resets the state when the key is fully released.
    if (value >= key.upperHysteresis && !key.continuousRapidTrigger)
        _heKeyStates[key.index].inRapidTriggerZone = false;
    // If continuous rapid trigger is enabled, the state is only reset to false when the key is fully released (<0.1mm).
    else if (value >= TRAVEL_DISTANCE_IN_0_01MM - CONTINUOUS_RAPID_TRIGGER_THRESHOLD && key.continuousRapidTrigger)
        _heKeyStates[key.index].inRapidTriggerZone = false;

    // RT STEP 2: If the value entered the rapid trigger zone, perform a press and set the rapid trigger state to true.
    // If the value is below the lower hysteresis and the rapid trigger state is false on the key, press the key because the action of entering
    // the rapid trigger zone is already counted as a trigger. From there on, the actuation point moves dynamically in that zone.
    // Also the rapid trigger state for the key has to be set to true in order to be processed by furture loops.
    if (value <= key.lowerHysteresis && !_heKeyStates[key.index].inRapidTriggerZone)
    {
        pressKey(key);
        _heKeyStates[key.index].inRapidTriggerZone = true;
    }

    // RT STEP 3: If the key *already is* in the rapid trigger zone (hence the 'else if'), check whether the key has travelled the sufficient amount.
    // Check whether the key should be pressed. This is the case if the key is currently not pressed,
    // the rapid trigger state is true and the value drops more than (down sensitivity) below the highest recorded value.
    else if (!_heKeyStates[key.index].pressed && _heKeyStates[key.index].inRapidTriggerZone && value + key.rapidTriggerDownSensitivity <= _heKeyStates[key.index].rapidTriggerPeak)
        pressKey(key);
    // Check whether the key should be released. This is the case if the key is currently pressed down and either the
    // rapid trigger state is no longer true or the value rises more than (up sensitivity) above the lowest recorded value.
    else if (_heKeyStates[key.index].pressed && (!_heKeyStates[key.index].inRapidTriggerZone || value >= _heKeyStates[key.index].rapidTriggerPeak + key.rapidTriggerUpSensitivity))
        releaseKey(key);

    // RT STEP 4: Always remember the peaks of the values, depending on the current pressed state.
    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((_heKeyStates[key.index].pressed && value < _heKeyStates[key.index].rapidTriggerPeak) || (!_heKeyStates[key.index].pressed && value > _heKeyStates[key.index].rapidTriggerPeak))
        _heKeyStates[key.index].rapidTriggerPeak = value;
}

void KeypadHandler::checkDigitalKey(const DigitalKey &key, bool pressed)
{
    // Check whether the key is pressed and send the HID command.
    if (pressed && millis() - _digitalKeyStates[key.index].lastDebounce >= DIGITAL_DEBOUNCE_DELAY)
    {
        pressKey(key);
        _digitalKeyStates[key.index].lastDebounce = millis();
    }
    else if(!pressed)
        releaseKey(key);
}

void KeypadHandler::pressKey(const Key &key)
{
    // Get the pointer to the correct pressed bool depending on the key type.
    // In case the key type is neither digital or hall effect (which shouldn't happen),
    // it defaults to a bool pointer to true, therefore the function exists out further down.
    bool *pressed = key.type == KeyType::HallEffect ? &_heKeyStates[key.index].pressed
                  : key.type == KeyType::Digital ? &_digitalKeyStates[key.index].pressed : nullptr;

    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!pressed || *pressed || !key.hidEnabled)
        return;

    // Send the HID instruction to the computer.
    *pressed = true;
    Keyboard.press(key.keyChar);
}

void KeypadHandler::releaseKey(const Key &key)
{
    // Get the pointer to the correct pressed bool depending on the key type.
    // In case the key type is neither digital or hall effect (which shouldn't happen),
    // it defaults to a null pointer, therefore the function exists out further down.
    bool *pressed = key.type == KeyType::HallEffect ? &_heKeyStates[key.index].pressed
                  : key.type == KeyType::Digital ? &_digitalKeyStates[key.index].pressed : nullptr;

    // Check whether the key is already pressed or HID commands are not enabled on the key.
    if (!pressed || !*pressed)
        return;

    // Send the HID instruction to the computer.
    Keyboard.release(key.keyChar);
    *pressed = false;
}

uint16_t KeypadHandler::readKey(const Key &key)
{
    // Perform a digital read if the key is a digital one.
    if (key.type == KeyType::Digital)
    {
        // Read the digital key and return 0 or 1 depending on whether the signal is HIGH or LOW.
        return digitalRead(DIGITAL_PIN(key.index));
    }
    // Perform an analog read if the key is a hall effect one.
    else if (key.type == KeyType::HallEffect)
    {
        // Read the value from the port of the specified key.
        uint16_t value = analogRead(HE_PIN(key.index));

        // Invert the value if the definition is set since in rare fields of application the sensor
        // is mounted the other way around, resulting in a different polarity and inverted sensor readings.
        // Since this firmware expects the value to go down when the button is pressed down, this is needed.
        // TODO: Replace 4095 with pow(2, ANALOG_RESOLUTION) - 1
#ifdef INVERT_SENSOR_READINGS
        value = 4095 - value;
#endif

        // Filter the value through the SMA filter and return it.
        return _heKeyStates[key.index].filter(value);
    }
    // Otherwise, in case anything goes wrong, default to 0.
    else
        return 0;
}

uint16_t KeypadHandler::mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const
{
    // Map the value with the calibrated down and rest position values to a range between 0 and TRAVEL_DISTANCE_IN_0_01MM and constrain it.
    // This is done to guarantee that the unit for the numbers used across the firmware actually matches the milimeter metric.
    return constrain(map(value, key.downPosition, key.restPosition, 0, TRAVEL_DISTANCE_IN_0_01MM), 0, TRAVEL_DISTANCE_IN_0_01MM);
}
