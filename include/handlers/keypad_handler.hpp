#pragma once

#include "config/configuration_controller.hpp"
#include "helpers/sma_filter.hpp"
#include "handlers/he_key_state.hpp"
#include "handlers/digital_key_state.hpp"
#include "definitions.hpp"

inline class KeypadHandler
{
public:
    KeypadHandler()
    {
        // Initialize the hall effect key states with their default values.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            _heKeyStates[i] = HEKeyState();

        // Initialize the digital key states with their default values.
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            _digitalKeyStates[i] = DigitalKeyState();
    }

    void handle();
    bool outputMode;

private:
    HEKeyState _heKeyStates[HE_KEYS];
    DigitalKeyState _digitalKeyStates[DIGITAL_KEYS];

    void checkHEKey(const HEKey &key, uint16_t value);
    void checkDigitalKey(const DigitalKey &key, bool pressed);
    void pressHEKey(const HEKey &key);
    void releaseHEKey(const HEKey &key);
    void pressDigitalKey(const DigitalKey &key);
    void releaseDigitalKey(const DigitalKey &key);
    uint16_t readHEKey(const HEKey &key);
    bool readDigitalKey(const DigitalKey &key);
    uint16_t mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const;
} KeypadHandler;
