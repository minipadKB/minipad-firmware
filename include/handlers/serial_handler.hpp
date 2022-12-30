#include <Arduino.h>
#include "config/configuration_controller.hpp"

class SerialHandler
{
public:
  SerialHandler(ConfigurationController *configController, char *firmwareVersion);
  virtual ~SerialHandler();

  void handleSerialInput(char *input);

private:
  bool handleCommand(char *command);
  void handleGet(char *key);
  void handleSet(char *key, char *value);

  ConfigurationController *configController;
  char *firmwareVersion;
};