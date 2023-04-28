#pragma once

#include "config/configuration_controller.hpp"
#include "helpers/sma_filter.hpp"
#include "handlers/he_key_state.hpp"
#include "definitions.hpp"

inline class KeypadHandler
{
public:
    KeypadHandler()
    {
        // Initialize the key states with their default values.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            _heKeyStates[i] = HEKeyState();
    }

    void handle();
    bool outputMode;

private:
    HEKeyState _heKeyStates[HE_KEYS];

    void checkHEKey(const HEKey &key, uint16_t value);
    void pressKey(const HEKey &key);
    void releaseKey(const HEKey &key);
    uint16_t readHEKey(const HEKey &key);
    uint16_t mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const;
} KeypadHandler;
