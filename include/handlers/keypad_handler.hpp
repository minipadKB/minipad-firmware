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

    uint16_t read(uint8_t keyIndex);
    uint16_t mapTo400Range(uint8_t keyIndex, uint16_t value);
    void checkTraditional(uint8_t keyIndex, uint16_t value);
    void checkRapidTrigger(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerPressKey(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value);
    void pressKey(uint8_t keyIndex);
    void releaseKey(uint8_t keyIndex);
} KeypadHandler;