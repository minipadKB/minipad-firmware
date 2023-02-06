#pragma once

#include "config/configuration.hpp"

class ConfigurationController
{
public:
    ConfigurationController(Configuration *defaultConfig, ToleranceConfiguration *tolerances);
    virtual ~ConfigurationController();

    void loadConfig();
    void saveConfig();

    Configuration *defaultConfig;
    Configuration config;
    ToleranceConfiguration *tolerances;
};