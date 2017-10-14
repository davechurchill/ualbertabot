/* 
 +----------------------------------------------------------------------+
 | UAlbertaBot                                                          |
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 | https://github.com/davechurchill/ualbertabot                         |
 +----------------------------------------------------------------------+
*/

// BWAPI Libraries
#include <BWAPI.h>
#include <BWAPI/Client.h>

// C++ Libraries
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

// 3rd Party Libraries
#include "rapidjson\document.h"


// UAlbertaBot Libraries
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "BWAPIOpponentView.h"
#include "debug\BWAPIPrintLogger.h"
#include "BotFactory.h"
#include "UAlbertaBot_Arena.h"
#include "UAlbertaBot_Tournament.h"
#include "BWAPIOpponentView.h"
#include "BWAPIMapInformation.h"

#include "debug\BWAPIPrintLogger.h"
#include "debug\BaseLocationManagerDebug.h"
#include "debug\GameCommanderDebug.h"
#include "debug\UnitInfoManagerDebug.h"
#include "debug\WorkerManagerDebug.h"
#include "debug\MapToolsDebug.h"
#include "debug\DebugInfoProvider.h"

using namespace AKBot;
using namespace UAlbertaBot;

AKBot::BWAPIOpponentView opponentView;
AKBot::BWAPIPrintLogger logger;

bool ConfigFileFound = true;
bool ConfigFileParsed = true;
std::string configurationFileName = "AK_Config.json";
BotConfiguration Config;

void UAlbertaBot_BWAPIReconnect() 
{
    while(!BWAPI::BWAPIClient.connect())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 }); 
    }
}

int main(int argc, const char * argv[]) 
{
    bool exitIfStarcraftShutdown = true;

    // parse the bot's configuration file, if it is not found or isn't valid, the program will exit
	auto configurationFile = ParseUtils::FindConfigurationLocation(configurationFileName);
	ParseUtils::ParseConfigFile(configurationFile, Config, ConfigFileFound, ConfigFileParsed);
	if (!ConfigFileFound)
	{
		std::cerr << "Error: Config File Not Found or is Empty\n";
		std::cerr << "Config Filename: " << configurationFile << "\n";
		std::cerr << "The bot will not run without its configuration file\n";
		std::cerr << "Please check that the file exists and is not empty. Incomplete paths are relative to the bot .exe file\n";
		std::cerr << "You can change the config file location in Config::ConfigFile::ConfigFileLocation\n";
	}

	if (!ConfigFileParsed)
	{
		std::cerr << "Error: Config File Found, but could not be parsed\n";
		std::cerr << "Config Filename: " << configurationFile << "\n";
		std::cerr << "The bot will not run without its configuration file\n";
		std::cerr << "Please check that the file exists, is not empty, and is valid JSON. Incomplete paths are relative to the bot .exe file\n";
		std::cerr << "You can change the config file location in Config::ConfigFile::ConfigFileLocation\n";		
	}

    size_t gameCount = 0;
	while (true)
    {
        // if we are not currently connected to BWAPI, try to reconnect
        if (!BWAPI::BWAPIClient.isConnected())
        {
            UAlbertaBot_BWAPIReconnect();
        }

        // if we have connected to BWAPI
        while (BWAPI::BWAPIClient.isConnected())
        {
            // wait for the game to start until the game starts
            std::cout << "Waiting for game start\n";
	        while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame()) 
            {
		        BWAPI::BWAPIClient.update();
	        }

            // Check to see if Starcraft shut down somehow
            if (BWAPI::BroodwarPtr == nullptr)
            {
                break;
            }

            // If we are successfully in a game, call the module to play the game
			if (BWAPI::Broodwar->isInGame())
			{
				std::cout << "Playing game " << gameCount++ << " on map " << BWAPI::Broodwar->mapFileName() << "\n";

				std::string mode = Config.BotInfo.BotMode;
				auto m = createBot(mode, Config, configurationFile);
				std::cerr << "Bot Address: " << m.getBot().get() << std::endl;
				if (!m.isValid())
				{
					std::cerr << "Unknown bot module selected: " << mode << "\n";
					exit(-1);
				}

				m.PlayGame();
            }
        }

        if (exitIfStarcraftShutdown && !BWAPI::BWAPIClient.isConnected())
        {
            return 0;
        }
    }

	return 0;
}
