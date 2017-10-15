#include "stdafx.h"
#include "CppUnitTest.h"
#include "BotConfiguration.h"
#include "ParseUtils.h"
#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<BWAPI::Race>(const BWAPI::Race& q)
{
	std::wstringstream _s;
	_s << "Race: " << q.getName().c_str();
	return _s.str();
}

inline std::string getLocalFile(std::string fileName)
{
	char cCurrentPath[FILENAME_MAX];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));

	std::string baseDirectory = cCurrentPath;
	baseDirectory = baseDirectory + "\\..\\..\\AkBot.Tests";
	return baseDirectory + "\\" + fileName;
}

namespace AkBotTests
{		
	TEST_CLASS(BotConfigurationTest)
	{
	public:
		
		TEST_METHOD(LoadingStrategyConfiguration)
		{
			BotConfiguration configuration;
			bool fileFound;
			bool configFileParsed;
			auto configFile = getLocalFile("AK_Config.json");
			UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

			Assert::AreEqual(true, fileFound, L"File not found");
			Assert::AreEqual(true, configFileParsed, L"Parsing error");
			Assert::AreEqual(3u, configuration.Strategy.EnemySpecificStrategy.size(), L"Should be found srtategies for 3 specific bots");
			
			Assert::AreEqual(13u, configuration.Strategy.Strategies.size(), L"Should be found srtategies for 13 predefined strategies");
		
			auto hatchMuta = configuration.Strategy.Strategies["Zerg_2HatchHydra"];
			Assert::AreEqual(BWAPI::Races::Zerg, hatchMuta.getRace(), L"Expected Zerg race");
			Assert::AreEqual(40u, hatchMuta.size(), L"Expected 40 steps deep");
		}

		TEST_METHOD(InvalidJsonShouldFailParsing)
		{
			BotConfiguration configuration;
			bool fileFound;
			bool configFileParsed;
			auto configFile = getLocalFile("Config_WithError.json");
			UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

			Assert::AreEqual(true, fileFound, L"File not found");
			Assert::AreEqual(false, configFileParsed, L"Somehow parsing error was not raised");
		}

		TEST_METHOD(NotExistingFileMark)
		{
			BotConfiguration configuration;
			bool fileFound;
			bool configFileParsed;
			auto configFile = getLocalFile("UnexistingFlag.json");
			UAlbertaBot::ParseUtils::ParseConfigFile(configFile, configuration, fileFound, configFileParsed);

			Assert::AreEqual(false, fileFound, L"File not found");
		}
	};
}