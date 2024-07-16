#pragma once

#include <toml/toml.hpp>
#include <string>


#define CONFIGURATIONLOG "Configuration"

class Config
{
public:
	Config(std::string ConfigName);

	int windowSizeX = 0;
	int windowSizeY = 0;

protected:

	toml::value configData;

};

