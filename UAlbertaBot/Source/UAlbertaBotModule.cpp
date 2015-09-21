/* 
 +----------------------------------------------------------------------+
 | UAlbertaBot                                                          |
 +----------------------------------------------------------------------+
 | University of Alberta - AIIDE StarCraft Competition                  |
 +----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------+
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 +----------------------------------------------------------------------+
*/

#include "Common.h"
#include "UAlbertaBotModule.h"
#include "JSONTools.h"
#include "ParseUtils.h"

using namespace UAlbertaBot;

// This gets called when the bot starts!
void UAlbertaBotModule::onStart()
{
    // Initialize SparCraft, the combat simulation package
    SparCraft::init();

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
    ParseUtils::ParseConfigFile("bwapi-data/AI/UAlbertaBot_Config.txt");

    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip);
    
    if (Config::BWAPIOptions::EnableCompleteMapInformation)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
    }

    if (Config::BWAPIOptions::EnableUserInput)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    }

    if (Config::BotInfo::PrintInfoOnStart)
    {
        BWAPI::Broodwar->printf("Hello! I am %s, written by %s", Config::BotInfo::BotName.c_str(), Config::BotInfo::Authors.c_str());
    }

    // Call BWTA to read and analyze the current map
    if (Config::Modules::UsingGameCommander)
	{
		BWTA::readMap();
		BWTA::analyze();
	}
}

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	if (Config::Modules::UsingGameCommander)
	{
		StrategyManager::Instance().onEnd(isWinner);
        
		ProductionManager::Instance().onGameEnd();
	}	
}

void UAlbertaBotModule::onFrame()
{
	if (Config::Modules::UsingGameCommander) 
	{ 
		_gameCommander.update(); 
	}

    if (Config::Modules::UsingAutoObserver)
    {
        _autoObserver.onFrame();
    }
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitDestroy(unit); }
}

void UAlbertaBotModule::onUnitMorph(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitMorph(unit); }
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	ParseUtils::ParseTextCommand(text);
}

void UAlbertaBotModule::onUnitCreate(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitCreate(unit); }
}

void UAlbertaBotModule::onUnitComplete(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitComplete(unit); }
}

void UAlbertaBotModule::onUnitShow(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { _gameCommander.onUnitRenegade(unit); }
}

