#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "InformationManager.h"
#include "MapGrid.h"
#include "base/WorkerManager.h"
#include "base/ProductionManager.h"
#include "base/BuildingManager.h"
#include "ScoutManager.h"
#include "StrategyManager.h"
#include "HLCombatCommander.h"

namespace UAlbertaBot
{

class TimerManager
{

	std::vector<BOSS::Timer> timers;
	std::vector<std::string> timerNames;

	int barWidth;

public:

	enum Type { All, Worker, Production, Building, Combat, Scout, InformationManager, MapGrid, MapTools, Search, NumTypes };


	TimerManager() : timers(std::vector<BOSS::Timer>(NumTypes)), barWidth(40)
	{
		timerNames.push_back("Total");
		timerNames.push_back("Worker");
		timerNames.push_back("Production");
		timerNames.push_back("Building");
		timerNames.push_back("Combat");
		timerNames.push_back("Scout");
		timerNames.push_back("UnitInfo");
		timerNames.push_back("MapGrid");
		timerNames.push_back("MapTools");
		timerNames.push_back("Search");
	}

	~TimerManager() {}

	void startTimer(const TimerManager::Type t)
	{
		timers[t].start();
	}

	void stopTimer(const TimerManager::Type t)
	{
		timers[t].stop();
	}

	double getTotalElapsed()
	{
		return timers[0].getElapsedTimeInMilliSec();
	}

	void displayTimers(int x, int y)
	{
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxScreen(x-5, y-5, x+110+barWidth, y+5+(10*timers.size()), BWAPI::Colors::Black, true);

		int yskip = 0;
		double total = timers[0].getElapsedTimeInMilliSec();
		for (size_t i(0); i<timers.size(); ++i)
		{
			double elapsed = timers[i].getElapsedTimeInMilliSec();
            if (elapsed > 55)
            {
                BWAPI::Broodwar->printf("Timer Debug: %s %lf", timerNames[i].c_str(), elapsed);
            }

			int width = (int)((elapsed == 0) ? 0 : (barWidth * (elapsed / total)));

			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y+yskip-3, "\x04 %s", timerNames[i].c_str());
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxScreen(x+60, y+yskip, x+60+width+1, y+yskip+8, BWAPI::Colors::White);
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+70+barWidth, y+yskip-3, "%.4lf", elapsed);
			yskip += 10;
		}
	}
};

class UnitToAssign
{
public:

	BWAPI::UnitInterface* unit;
	bool isAssigned;

	UnitToAssign(BWAPI::UnitInterface* u)
	{
		unit = u;
		isAssigned = false;
	}
};

class GameCommander 
{
	CombatCommander		combatCommander;
	HLCombatCommander	hlCombatCommander;

	ScoutManager		scoutManager;
	TimerManager		timerManager;

	std::set<BWAPI::UnitInterface*> combatUnits;
	std::set<BWAPI::UnitInterface*> scoutUnits;
	std::set<BWAPI::UnitInterface*> workerUnits;

	std::set<BWAPI::UnitInterface*>	validUnits;
	std::set<BWAPI::UnitInterface*> assignedUnits;

	BWAPI::UnitInterface* currentScout;
	int numWorkerScouts;

	const bool isAssigned(BWAPI::UnitInterface* unit) const;

public:

	GameCommander();
	~GameCommander() {};

	void update();

	void populateUnitVectors();
	void setValidUnits();
	void setScoutUnits();
	void setWorkerUnits();
	void setCombatUnits();

	void drawDebugInterface();

	bool isValidUnit(BWAPI::UnitInterface* unit);
	bool isCombatUnit(BWAPI::UnitInterface* unit) const;

	BWAPI::UnitInterface* getFirstSupplyProvider();
	BWAPI::UnitInterface* getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);
	BWAPI::UnitInterface* getClosestWorkerToTarget(BWAPI::Position target);

	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void onUnitDestroy(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
};

}