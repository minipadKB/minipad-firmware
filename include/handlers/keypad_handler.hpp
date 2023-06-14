#pragma once

#include "config/configuration_controller.hpp"
#include "helpers/sma_filter.hpp"
#include "handlers/key_states/he_key_state.hpp"
#include "handlers/key_states/digital_key_state.hpp"
#include "definitions.hpp"

inline class KeypadHandler
{
public:
    KeypadHandler()
    {
        // Initialize the hall effect key states with their default values.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            heKeyStates[i] = HEKeyState();

        // Initialize the digital key states with their default values.
#pragma GCC diagnostic ignored "-Wtype-limits"
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
#pragma GCC diagnostic pop
            digitalKeyStates[i] = DigitalKeyState();
    }

    void handle();
    bool outputMode;
    HEKeyState heKeyStates[HE_KEYS];
    DigitalKeyState digitalKeyStates[DIGITAL_KEYS];

private:
    void checkHEKey(const HEKey &key, uint16_t value);
    void checkDigitalKey(const DigitalKey &key, bool pressed);
    void pressKey(const Key &key);
    void releaseKey(const Key &key);
    uint16_t readKey(const Key &key);
    uint16_t mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const;
} KeypadHandler;
