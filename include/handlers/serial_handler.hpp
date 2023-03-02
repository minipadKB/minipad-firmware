#include "config/configuration_controller.hpp"

class SerialHandler
{
public:
    SerialHandler(ConfigurationController *configController);
    virtual ~SerialHandler();

    void handleSerialInput(String *inputStr);

private:
    void handleGlobalSetting(char *key, char *value);
    void handleKeySetting(uint8_t keyIndex, char *key, char *value);

    bool isEqual(char *str1, const char *str2);

    ConfigurationController *configController;
};