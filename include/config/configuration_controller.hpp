#pragma once

#include "config/configuration.hpp"

class ConfigurationController
{
public:
    ConfigurationController(Configuration *defaultConfig);
    virtual ~ConfigurationController();

    void loadConfig();
    void saveConfig();

    Configuration *defaultConfig;
    Configuration config;
};