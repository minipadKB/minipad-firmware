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
#else
  bool pressedStates[2];
#endif

  uint16_t read(uint8_t keyIndex);
  void checkTraditionalMode(uint8_t keyIndex);
  void checkRapidTriggerMode(uint8_t keyIndex);
  void pressKey(uint8_t keyIndex);
  void releaseKey(uint8_t keyIndex);
};