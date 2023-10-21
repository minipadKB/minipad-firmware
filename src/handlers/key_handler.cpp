#include <Arduino.h>
#include <Keyboard.h>
#include "handlers/key_handler.hpp"
#include "handlers/serial_handler.hpp"
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

void KeyHandler::handle()
{
    // Go through all Hall Effect keys and run the checks.
    for (HEKey &key : heKeys)
    {
        // Scan the Hall Effect key to update the sensor and distance value.
        scanHEKey(key);

        // Run the checks on the HE key.
        checkHEKey(key);
    }

    // Go through all digital keys and run the checks.
    for (DigitalKey &key : digitalKeys)
    {
        // Scan the digital key to update the pin status.
        scanDigitalKey(key);

        // Run the checks on the digital key.
        checkDigitalKey(key);
    }

    // Send the key report via the HID interface after updating the report.
    Keyboard.sendReport();
}

void KeyHandler::updateSensorBoundaries(HEKey &key)
{
    // Calculate the value with the deadzone in the positive and negative direction applied.
    uint16_t upperValue = key.rawValue - SENSOR_BOUNDARY_DEADZONE;
    uint16_t lowerValue = key.rawValue + SENSOR_BOUNDARY_DEADZONE;

    // If the read value with deadzone applied is bigger than the current rest position, update it.
    if (key.restPosition < upperValue)
        key.restPosition = upperValue;

    // If the read value with deadzone applied is lower than the current down position, update it. Make sure that the distance to the rest position
    // is at least SENSOR_BOUNDARY_MIN_DISTANCE (scaled with travel distance @ 4.00mm) to prevent poor calibration/analog range resulting in "crazy behaviour".
    else if (key.downPosition > lowerValue &&
             key.restPosition - lowerValue >= SENSOR_BOUNDARY_MIN_DISTANCE * TRAVEL_DISTANCE_IN_0_01MM / 400)
    {
        // From here on, the down position has been set < rest position, therefore the key can be considered calibrated, allowing distance calculation.
        key.calibrated = true;

        key.downPosition = lowerValue;
    }
}

void KeyHandler::scanHEKey(HEKey &key)
{
    // Read the value from the port of the specified key and run it through the SMA filter.
    key.rawValue = key.filter(analogRead(HE_PIN(key.index)));

    // Invert the value if the definition is set since in rare fields of application the sensor
    // is mounted the other way around, resulting in a different polarity and inverted sensor readings.
    // Since this firmware expects the value to go down when the button is pressed down, this is needed.
#ifdef INVERT_SENSOR_READINGS
    key.rawValue = (1 << ANALOG_RESOLUTION) - 1 - key.rawValue;
#endif

    // If the SMA filter is fully initalized (at least one full circular buffering has been performed), calibration can be performed.
    // This keeps track of the lowest and highest value reached on each key, giving us boundaries to map to an actual milimeter distance.
    if (key.filter.initialized)
        updateSensorBoundaries(key);

    // Make sure that the key is calibrated, which means that the down position (default 4095) was updated to be  smaller than the rest position.
    // If that's not the case, we go with the total switch travel distance representing a key that is fully up, effectively disabling any value processing.
    // This if-branch is inheritly triggered if the SMA filter is not initialized yet, as the default down position of 4095 was not updated yet.
    if(!key.calibrated)
    {
        key.distance = TRAVEL_DISTANCE_IN_0_01MM;
        return;
    }

#ifdef USE_GAUSS_CORRECTION_LUT

    // If gauss correction is enabled, use the GaussLUT instance to get the distance based on the adc value and the rest position
    // of the key, which is used to determine the offset from the "ideal" rest position set by the lookup table calculations.
    uint16_t distance = gaussLUT.adcToDistance(key.rawValue, key.restPosition);

    // Stretch the value to the full travel distance using our down position since the LUT is rest-position based. Then invert and constrain it.
    distance = distance * TRAVEL_DISTANCE_IN_0_01MM / gaussLUT.adcToDistance(key.downPosition, key.restPosition);
    key.distance = constrain(TRAVEL_DISTANCE_IN_0_01MM - distance, 0, TRAVEL_DISTANCE_IN_0_01MM);

#else

    // Map the value with the down and rest position values to a range between 0 and TRAVEL_DISTANCE_IN_0_01MM and constrain it.
    // This is done to guarantee that the unit for the numbers used across the firmware actually matches the milimeter metric.
    // NOTE: This calcuation disregards the non-linear nature of the relation between a magnet's distance and it's magnetic field strength.
    //       This firmware has a gauss correction, which can be enabled and adjusted to match the hardware specifications of the device.
    return constrain(map(value, key.downPosition, key.restPosition, 0, TRAVEL_DISTANCE_IN_0_01MM), 0, TRAVEL_DISTANCE_IN_0_01MM);

#endif
}

void KeyHandler::scanDigitalKey(DigitalKey &key)
{
    // Read the digital key and save the pin status in the key.
    key.isHigh = digitalRead(DIGITAL_PIN(key.index)) == PinStatus::HIGH;
}

void KeyHandler::checkHEKey(HEKey &key)
{
    // If the key is in traditional mode, do the usual hysteresis checks.
    if (!key.config->rapidTrigger)
    {
        // Check whether the value passes the lower or upper hysteresis.
        // If the value drops <= the lower hysteresis, the key is pressed down.
        // If the value rises >= the upper hysteresis, the key is released.
        if (key.distance <= key.config->lowerHysteresis)
            setPressedState(key, true);
        else if (key.distance >= key.config->upperHysteresis)
            setPressedState(key, false);

        // Return here to not run into the rapid trigger code.
        return;
    }

    // RT STEP 1: Reset the rapid trigger state if the value left the rapid trigger zone (normal) or was fully released (CRT).
    // If the value is above the upper hysteresis the value is not (anymore) inside the rapid trigger zone
    // meaning the rapid trigger state for the key has to be set to false in order to be processed by further checks.
    // This only applies if continuous rapid trigger is not enabled as it only resets the state when the key is fully released.
    if (key.distance >= key.config->upperHysteresis && !key.config->continuousRapidTrigger)
        key.inRapidTriggerZone = false;
    // If continuous rapid trigger is enabled, the state is only reset to false when the key is fully released (<0.1mm).
    else if (key.distance >= TRAVEL_DISTANCE_IN_0_01MM - CONTINUOUS_RAPID_TRIGGER_THRESHOLD && key.config->continuousRapidTrigger)
        key.inRapidTriggerZone = false;

    // RT STEP 2: If the value entered the rapid trigger zone, perform a press and set the rapid trigger state to true.
    // If the value is below the lower hysteresis and the rapid trigger state is false on the key, press the key because the action of entering
    // the rapid trigger zone is already counted as a trigger. From there on, the actuation point moves dynamically in that zone.
    // Also the rapid trigger state for the key has to be set to true in order to be processed by furture loops.
    if (key.distance <= key.config->lowerHysteresis && !key.inRapidTriggerZone)
    {
        setPressedState(key, true);
        key.inRapidTriggerZone = true;
    }

    // RT STEP 3: If the key *already is* in the rapid trigger zone (hence the 'else if'), check whether the key has travelled the sufficient amount.
    // Check whether the key should be pressed. This is the case if the key is currently not pressed,
    // the rapid trigger state is true and the value drops more than (down sensitivity) below the highest recorded value.
    else if (!key.pressed && key.inRapidTriggerZone && key.distance + key.config->rapidTriggerDownSensitivity <= key.rapidTriggerPeak)
        setPressedState(key, true);
    // Check whether the key should be released. This is the case if the key is currently pressed down and either the
    // rapid trigger state is no longer true or the value rises more than (up sensitivity) above the lowest recorded value.
    else if (key.pressed && (!key.inRapidTriggerZone || key.distance >= key.rapidTriggerPeak + key.config->rapidTriggerUpSensitivity))
        setPressedState(key, false);

    // RT STEP 4: Always remember the peaks of the values, depending on the current pressed state.
    // If the key is pressed and at an all-time low or not pressed and at an all-time high, save the value.
    if ((key.pressed && key.distance < key.rapidTriggerPeak) || (!key.pressed && key.distance > key.rapidTriggerPeak))
        key.rapidTriggerPeak = key.distance;
}

void KeyHandler::checkDigitalKey(DigitalKey &key)
{
    // Check whether the pin status on the key is HIGH and the key is fully debounced.
    if (key.isHigh && millis() - key.lastDebounce >= DIGITAL_DEBOUNCE_DELAY)
    {
        // Set the key to pressed and update the last debounce time.
        setPressedState(key, true);
        key.lastDebounce = millis();
    }
    // If the key is not pressed, just set it to unpressed.
    else if (!key.isHigh)
        setPressedState(key, false);
}

void KeyHandler::setPressedState(Key &key, bool pressed)
{
    // Check whether the pressed state changes or HID is not enabled.
    if (key.pressed == pressed || !key.config->hidEnabled)
        return;

    // Send the HID instruction to the computer.
    if (pressed)
        Keyboard.press(key.config->keyChar);
    else
        Keyboard.release(key.config->keyChar);

    // Update the pressed value state.
    key.pressed = pressed;
}
