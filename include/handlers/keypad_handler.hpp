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
            _heKeyStates[i] = HEKeyState();

        // Initialize the digital key states with their default values.
#pragma GCC diagnostic ignored "-Wtype-limits"
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
#pragma GCC diagnostic pop
            _digitalKeyStates[i] = DigitalKeyState();
    }

    void handle();
    bool outputMode;

private:
    HEKeyState _heKeyStates[HE_KEYS];
    DigitalKeyState _digitalKeyStates[DIGITAL_KEYS];

    void checkHEKey(const HEKey &key, uint16_t value);
    void checkDigitalKey(const DigitalKey &key, bool pressed);
    void pressKey(const Key &key, bool isHeKey);
    void releaseKey(const Key &key, bool isHeKey);
    uint16_t readHEKey(const HEKey &key);
    bool readDigitalKey(const DigitalKey &key);
    uint16_t mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const;
} KeypadHandler;
