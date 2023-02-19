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

    bool keyPressedStates[KEYS];
    uint16_t currentRapidTriggerPeak[KEYS];

    uint16_t read(uint8_t keyIndex);
    void checkTraditional(uint8_t keyIndex, uint16_t value);
    void checkRapidTrigger(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerPressKey(uint8_t keyIndex, uint16_t value);
    bool checkRapidTriggerReleaseKey(uint8_t keyIndex, uint16_t value);
    void pressKey(uint8_t keyIndex);
    void releaseKey(uint8_t keyIndex);
};