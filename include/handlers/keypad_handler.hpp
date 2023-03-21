#pragma once

#include "config/configuration_controller.hpp"
#include "helpers/sma_filter.hpp"
#include "handlers/key_state.hpp"
#include "definitions.hpp"

inline class KeypadHandler
{
public:
    KeypadHandler()
    {
        // Initialize the SMA filters.
        for (uint8_t i = 0; i < KEYS; i++)
            _keyStates[i] = KeyState();
    }

    void handle();
    bool calibrationMode;

private:
    KeyState _keyStates[KEYS];

    uint16_t read(Key key);
    uint16_t mapTo400Range(Key key, uint16_t value);
    void checkTraditional(Key key, uint16_t value);
    void checkRapidTrigger(Key key, uint16_t value);
    bool checkRapidTriggerPressKey(Key key, uint16_t value);
    bool checkRapidTriggerReleaseKey(Key key, uint16_t value);
    void pressKey(Key key);
    void releaseKey(Key key);
} KeypadHandler;