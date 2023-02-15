#include "config/configuration_controller.hpp"
#include "definitions.hpp"

class KeypadHandler
{
public:
    KeypadHandler(ConfigurationController *configController);
    virtual ~KeypadHandler();

    void check();

private:
    ConfigurationController *configController;

#ifdef KEYS_3
    bool keyPressedStates[3];
    uint16_t currentRapidTriggerPeak[3];
#else
    bool keyPressedStates[2];
    uint16_t currentRapidTriggerPeak[2];
#endif

    uint16_t read(uint8_t keyIndex);
    void checkTraditional(uint8_t keyIndex, uint16_t value);
    void checkRapidTrigger(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerPressKey(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value);
    void pressKey(uint8_t keyIndex);
    void releaseKey(uint8_t keyIndex);
};