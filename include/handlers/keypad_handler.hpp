#pragma once

#include "config/configuration_controller.hpp"
#include "definitions.hpp"

inline class KeypadHandler
{
public:
    void check();
    bool calibrationMode;

private:
    bool _keyPressedStates[KEYS];
    bool _rapidTriggerEnabled[KEYS];
    uint16_t _currentRapidTriggerPeak[KEYS];
    uint16_t _lastValues[KEYS][16];
    uint8_t _nextLastValuesIndex[KEYS];

    uint16_t read(Key key);
    uint16_t mapTo400Range(Key key, uint16_t value);
    void checkTraditional(Key key, uint16_t value);
    void checkRapidTrigger(Key key, uint16_t value);
    bool checkRapidTriggerPressKey(Key key, uint16_t value);
    bool checkRapidTriggerReleaseKey(Key key, uint16_t value);
    void pressKey(Key key);
    void releaseKey(Key key);
} KeypadHandler;