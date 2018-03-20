#include <direct.h>
#define BOOST_TEST_MODULE BotConfigurationTest
#include <boost/test/included/unit_test.hpp>
#include "BotConfiguration.h"
#include "ParseUtils.h"

inline std::string getLocalFile(std::string fileName)
{
	char cCurrentPath[FILENAME_MAX];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));

	std::string baseDirectory = cCurrentPath;
#if WITH_CMAKE
	baseDirectory = baseDirectory + "\\..";
#else
	baseDirectory = baseDirectory + "\\..\\..\\..\\AkBot.Tests";
#endif
	return baseDirectory + "\\" + fileName;
}

BOOST_AUTO_TEST_CASE(LoadingStrategyConfiguration)
{
	BotConfiguration configuration;
	bool fileFound;
	bool configFileParsed;
	auto configFile = getLocalFile("AK_Config.json");
	UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

	BOOST_TEST(true == fileFound, L"File not found");
	BOOST_TEST(true == configFileParsed, L"Parsing error");
	BOOST_TEST(3u == configuration.Strategy.EnemySpecificStrategy.size(), L"Should be found srtategies for 3 specific bots");

	BOOST_TEST(13u == configuration.Strategy.Strategies.size(), L"Should be found srtategies for 13 predefined strategies");

	auto hatchMuta = configuration.Strategy.Strategies["Zerg_2HatchHydra"];
	BOOST_TEST(BWAPI::Races::Zerg == hatchMuta.getRace(), L"Expected Zerg race");
	BOOST_TEST(40u == hatchMuta.size(), L"Expected 40 steps deep");
}

BOOST_AUTO_TEST_CASE(InvalidJsonShouldFailParsing)
{
	BotConfiguration configuration;
	bool fileFound;
	bool configFileParsed;
	auto configFile = getLocalFile("Config_WithError.json");
	UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

	BOOST_TEST(true == fileFound, L"File not found");
	BOOST_TEST(false == configFileParsed, L"Somehow parsing error was not raised");
}

BOOST_AUTO_TEST_CASE(NotExistingFileMark)
{
	BotConfiguration configuration;
	bool fileFound;
	bool configFileParsed;
	auto configFile = getLocalFile("UnexistingFlag.json");
	UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

	BOOST_TEST(false == fileFound, L"File not found");
}
