#pragma once
#pragma GCC diagnostic ignored "-Wtype-limits"

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
        for (uint8_t i = 0; i < DIGITAL_KEYS; i++)
            digitalKeyStates[i] = DigitalKeyState();
    }

    void handle();
    bool outputMode;
    HEKeyState heKeyStates[HE_KEYS];
    DigitalKeyState digitalKeyStates[DIGITAL_KEYS];

private:
    const double lutPramA = 6647.8446648;
    const double lutPramB = -0.00609446727442;
    const double lutPramC = -721.743991123;
    const double lutPramD = 4525.58542876;
    uint16_t lut[1 << ANALOG_RESOLUTION];

    uint16_t adcReadingToDistance(uint16_t adcReading);
    uint16_t distanceToAdcReading(uint16_t distance);
    void getSensorOffsets(const Key &key);
    void applyCalibrationToRawAdcReading(const HEKey &key, uint16_t value);
    void generate_lut(void);
    void calibrate(void);

    void updateSensorBoundaries(void);
    void checkHEKey(const HEKey &key, uint16_t value);
    void checkDigitalKey(const DigitalKey &key, bool pressed);
    void pressKey(const Key &key);
    void releaseKey(const Key &key);
    uint16_t readKey(const Key &key);
    uint16_t mapSensorValueToTravelDistance(const HEKey &key, uint16_t value) const;
} KeypadHandler;
