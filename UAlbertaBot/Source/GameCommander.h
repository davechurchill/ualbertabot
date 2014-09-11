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

	BWAPI::Unit * unit;
	bool isAssigned;

	UnitToAssign(BWAPI::Unit * u)
	{
		unit = u;
		isAssigned = false;
	}
};

class GameCommander 
{
	CombatCommander		combatCommander;
	ScoutManager		scoutManager;
	TimerManager		timerManager;

	std::set<BWAPI::Unit *> combatUnits;
	std::set<BWAPI::Unit *> scoutUnits;
	std::set<BWAPI::Unit *> workerUnits;

	std::set<BWAPI::Unit *>	validUnits;
	std::set<BWAPI::Unit *> assignedUnits;

	BWAPI::Unit * currentScout;
	int numWorkerScouts;

	const bool isAssigned(BWAPI::Unit * unit) const;

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

	bool isValidUnit(BWAPI::Unit * unit);
	bool isCombatUnit(BWAPI::Unit * unit) const;

	BWAPI::Unit * getFirstSupplyProvider();
	BWAPI::Unit * getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);
	BWAPI::Unit * getClosestWorkerToTarget(BWAPI::Position target);

	void onUnitShow(BWAPI::Unit * unit);
	void onUnitHide(BWAPI::Unit * unit);
	void onUnitCreate(BWAPI::Unit * unit);
	void onUnitRenegade(BWAPI::Unit * unit);
	void onUnitDestroy(BWAPI::Unit * unit);
	void onUnitMorph(BWAPI::Unit * unit);
};

}