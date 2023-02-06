#include "config/configuration_controller.hpp"

class SerialHandler
{
public:
    SerialHandler(ConfigurationController *configController, char *firmwareVersion);
    virtual ~SerialHandler();

    void handleSerialInput(String *inputStr);

private:
    void handleCommand(char *command);
    void handleSet(char *key, char *value);

    bool isEqual(char *str1, const char *str2);

    ConfigurationController *configController;
    char *firmwareVersion;
};