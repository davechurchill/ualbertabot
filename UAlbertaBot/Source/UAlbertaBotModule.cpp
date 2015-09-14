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

using namespace UAlbertaBot;

void UAlbertaBotModule::onStart()
{
	BWAPI::Broodwar->setLocalSpeed(0);
	BWAPI::Broodwar->setFrameSkip(10);

    SparCraft::init();
    BOSS::init();

	BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    Options::BotModes::SetBotMode(Options::BotModes::AIIDE_TOURNAMENT);
	Options::Modules::checkOptions();
	
    if (Options::Modules::USING_GAMECOMMANDER)
	{
		BWTA::readMap();
		BWTA::analyze();
	}
}

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	if (Options::Modules::USING_GAMECOMMANDER)
	{
		StrategyManager::Instance().onEnd(isWinner);
        
		ProductionManager::Instance().onGameEnd();
	}	

	if (isWinner)
	{
		Logger::LogAppendToFile(UAB_LOGFILE, "WON! :)");
	}
	else
	{
		Logger::LogAppendToFile(UAB_LOGFILE, "LOST! :(");
	}
}

void UAlbertaBotModule::onFrame()
{
	if (Options::Modules::USING_UNIT_COMMAND_MGR)
	{
		UnitCommandManager::Instance().update();
	}

	if (Options::Modules::USING_GAMECOMMANDER) 
	{ 
		gameCommander.update(); 
	}
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
}

void UAlbertaBotModule::onUnitMorph(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (Options::Modules::USING_BUILD_ORDER_DEMO)
	{
		std::stringstream type;
		std::stringstream numUnitType;
		size_t numUnits = 0;

		size_t i=0;
		for (i=0; i<text.length(); ++i)
		{
			if (text[i] == ' ')
			{
				i++;
				break;
			}

			type << text[i];
		}

		for (; i<text.length(); ++i)
		{
			numUnitType << text[i];
		}

		numUnits = atoi(numUnitType.str().c_str());

        BWAPI::UnitType t;
        for (const BWAPI::UnitType & tt : BWAPI::UnitTypes::allUnitTypes())
        {
            if (tt.getName().compare(type.str()) == 0)
            {
                t = tt;
                break;
            }
        }
	
		BWAPI::Broodwar->printf("Searching for %d of %s", numUnits, t.getName().c_str());

        if (t != BWAPI::UnitType())
        {
            MetaPairVector goal;
		    goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Probe, 8));
		    goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Gateway, 2));
		    goal.push_back(MetaPair(t, numUnits));

		    ProductionManager::Instance().setSearchGoal(goal);
        }
        else
        {
            BWAPI::Broodwar->printf("Unknown unit type %s", type.str().c_str());
        }

		
	}
}

void UAlbertaBotModule::onUnitCreate(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitCreate(unit); }
}

void UAlbertaBotModule::onUnitComplete(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitComplete(unit); }
}

void UAlbertaBotModule::onUnitShow(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
}
