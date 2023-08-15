#pragma once
#pragma GCC diagnostic ignored "-Wtype-limits"

#include "config/configuration_controller.hpp"
#include "handlers/key_states/he_key_state.hpp"
#include "handlers/key_states/digital_key_state.hpp"
#include "helpers/sma_filter.hpp"
#include "helpers/gauss_lut.hpp"
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
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            digitalKeyStates[i] = DigitalKeyState();
    }

    void handle();
    bool outputMode;
    HEKeyState heKeyStates[HE_KEYS];
    DigitalKeyState digitalKeyStates[DIGITAL_KEYS];

private:
    void updateSensorBoundaries(const HEKey &key, uint16_t value);
    void checkHEKey(const HEKey &key, uint16_t value);
    void checkDigitalKey(const DigitalKey &key, bool pressed);
    void pressKey(const Key &key);
    void releaseKey(const Key &key);
    uint16_t readKey(const Key &key);
    uint16_t adcToDistance(const HEKey &key, uint16_t value);

#ifdef USE_GAUSS_CORRECTION_LUT
    GaussLUT gaussLUT = GaussLUT(GAUSS_CORRECTION_PARAM_A, GAUSS_CORRECTION_PARAM_B, GAUSS_CORRECTION_PARAM_C, GAUSS_CORRECTION_PARAM_D);
#endif
} KeypadHandler;
