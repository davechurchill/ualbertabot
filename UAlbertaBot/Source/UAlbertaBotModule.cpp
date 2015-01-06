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

BWAPI::AIModule * __NewAIModule()
{
	return new UAlbertaBotModule();
}

UAlbertaBotModule::UAlbertaBotModule()  {}
UAlbertaBotModule::~UAlbertaBotModule() {}

void UAlbertaBotModule::onStart()
{
	BWAPI::Broodwar->setLocalSpeed(5);
	//BWAPI::Broodwar->setFrameSkip(0);

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
	
	if (Options::Modules::USING_MICRO_SEARCH)
	{
	    BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
		SparCraft::init();
		
		sparcraftManager.onStart();
	}
}

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	if (Options::Modules::USING_GAMECOMMANDER)
	{
		StrategyManager::Instance().onEnd(isWinner);
        
		ProductionManager::Instance().onGameEnd();
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
	
	if (Options::Modules::USING_ENHANCED_INTERFACE)
	{
		eui.update();
	}

	if (Options::Modules::USING_MICRO_SEARCH)
	{
		sparcraftManager.update();
	}

	if (Options::Modules::USING_REPLAY_VISUALIZER)
	{
		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
		{
			BWAPI::Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "   %d", unit->getPlayer()->getID());

			if (unit->isSelected())
			{
				BWAPI::Broodwar->drawCircleMap(unit->getPosition().x, unit->getPosition().y, 1000, BWAPI::Colors::Red);
			}
		}
	}
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
	if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }
}

void UAlbertaBotModule::onUnitMorph(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (Options::Modules::USING_REPLAY_VISUALIZER && (text.compare("sim") == 0))
	{
		BWAPI::UnitInterface* selected = NULL;
		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
		{
			if (unit->isSelected())
			{
				selected = unit;
				break;
			}
		}

		if (selected)
		{
			#ifdef USING_VISUALIZATION_LIBRARIES
				//ReplayVisualizer rv;
				//rv.launchSimulation(selected->getPosition(), 1000);
			#endif
		}
	}

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
        for (BWAPI::UnitType & tt : BWAPI::UnitTypes::allUnitTypes())
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
