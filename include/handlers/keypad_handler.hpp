#include "config/configuration_controller.hpp"
#include "config/tolerance_configuration.hpp"
#include "definitions.hpp"

class KeypadHandler
{
public:
    KeypadHandler(ConfigurationController *configController, ToleranceConfiguration *tolerances);
    virtual ~KeypadHandler();

    void check();

private:
    ConfigurationController *configController;
    ToleranceConfiguration *tolerances;

#ifdef KEYS_3
    bool pressedStates[3];
    uint16_t currentRapidTriggerPeak[3];
#else
    bool pressedStates[2];
    uint16_t lastRapidTriggerValues[2];
#endif

    uint16_t read(uint8_t keyIndex);
    void checkTraditional(uint8_t keyIndex, uint16_t value);
    void checkRapidTrigger(uint8_t keyIndex, uint16_t value);
    void pressKey(uint8_t keyIndex);
    void releaseKey(uint8_t keyIndex);
};