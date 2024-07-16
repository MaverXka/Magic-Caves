#include "Config.h"
#include "Logging/Logger.h"

Config::Config(std::string ConfigName)
{
	auto ConfigData = toml::try_parse(ConfigName, toml::spec::v(1, 1, 0));
	if (ConfigData.is_err())
	{
		LOG(CONFIGURATIONLOG, "Loading config failed");
		throw std::exception("Loading config failed");
	}
	else
	{
		configData = ConfigData.unwrap();
		windowSizeX = configData.at("windowSizeX").as_integer();
		windowSizeY = configData.at("windowSizeY").as_integer();
	}
}
