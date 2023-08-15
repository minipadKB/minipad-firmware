#pragma once
#pragma GCC diagnostic ignored "-Wtype-limits"

#include "config/configuration_controller.hpp"
#include "handlers/key_states/he_key_state.hpp"
#include "handlers/key_states/digital_key_state.hpp"
#include "helpers/sma_filter.hpp"
#include "helpers/gauss_lut.hpp"
#include "definitions.hpp"

inline class KeyHandler
{
public:
    KeyHandler()
    {
        // Assign indicies to all Hall Effect key states.
        for (uint8_t i = 0; i < HE_KEYS; i++)
            heKeyStates[i].index = i;

        // Assign indicies to all digital key states.
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            digitalKeyStates[i].index = i;
    }

    void handle();
    bool outputMode;
    HEKeyState heKeyStates[HE_KEYS];
    DigitalKeyState digitalKeyStates[DIGITAL_KEYS];

private:
    void updateSensorBoundaries(HEKeyState &keyState);
    void checkHEKey(const HEKey &key, HEKeyState &keyState);
    void checkDigitalKey(const DigitalKey &key, DigitalKeyState &keyState);
    void scanHEKey(HEKeyState &keyState);
    void scanDigitalKey(DigitalKeyState &keyState);
    void setPressedState(const Key &key, KeyState &keyState, bool pressed);

#ifdef USE_GAUSS_CORRECTION_LUT
    GaussLUT gaussLUT = GaussLUT(GAUSS_CORRECTION_PARAM_A, GAUSS_CORRECTION_PARAM_B, GAUSS_CORRECTION_PARAM_C, GAUSS_CORRECTION_PARAM_D);
#endif
} KeyHandler;
