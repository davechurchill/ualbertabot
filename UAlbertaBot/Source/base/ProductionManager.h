#pragma once

#include <Common.h>
#include "BuildOrderQueue.h"
#include "BuildingManager.h"
#include "StrategyManager.h"
#include "../BOSSManager.h"

namespace UAlbertaBot
{
typedef unsigned char Action;

class CompareWhenStarted 
{

public:

	CompareWhenStarted() {}

	// the sorting operator
	bool operator() (BWAPI::Unit * u1, BWAPI::Unit * u2) 
	{
		int startedU1 = BWAPI::Broodwar->getFrameCount() - (u1->getType().buildTime() - u1->getRemainingBuildTime());
		int startedU2 = BWAPI::Broodwar->getFrameCount() - (u2->getType().buildTime() - u2->getRemainingBuildTime());
		return startedU1 > startedU2;
    }
};

class ProductionManager 
{
	ProductionManager();

	bool						initialBuildSet;

	std::map<char, MetaType>	typeCharMap;
	std::vector<MetaPair>       searchGoal;

	bool						assignedWorkerForThisBuilding;
	bool						haveLocationForThisBuilding;
	int							reservedMinerals, reservedGas;
	bool						enemyCloakedDetected;
	bool						rushDetected;

	BWAPI::TilePosition			predictedTilePosition;
    BWAPI::Unit *               getClosestUnitToPosition(std::set<BWAPI::Unit *> & units, BWAPI::Position closestTo);
	BWAPI::Unit *				selectUnitOfType(BWAPI::UnitType type, BWAPI::Position closestTo = BWAPI::Position(0,0));
	BuildOrderQueue				queue;

	bool						contains(UnitVector & units, BWAPI::Unit * unit);
	void						populateTypeCharMap();
	bool						hasResources(BWAPI::UnitType type);
	bool						canMake(BWAPI::UnitType type);
	bool						hasNumCompletedUnitType(BWAPI::UnitType type, int num);
	bool						meetsReservedResources(MetaType type);
	void						setBuildOrder(const std::vector<MetaType> & buildOrder);
	void						createMetaType(BWAPI::Unit * producer, MetaType type);
	void						manageBuildOrderQueue();
	void						performCommand(BWAPI::UnitCommandType t);
	bool						canMakeNow(BWAPI::Unit * producer, MetaType t);
	void						predictWorkerMovement(const Building & b);

	bool						detectBuildOrderDeadlock();

	int							getFreeMinerals();
	int							getFreeGas();

public:

	static ProductionManager &	Instance();

	void						drawQueueInformation(std::map<BWAPI::UnitType, int> & numUnits, int x, int y, int index);
	void						update();

	void						onGameEnd();
	void						onUnitMorph(BWAPI::Unit * unit);
	void						onUnitDestroy(BWAPI::Unit * unit);
	
    
    BWAPI::Unit *               getProducer(MetaType t, BWAPI::Position closestTo = BWAPI::Positions::None);

	void						performBuildOrderSearch(const std::vector<MetaPair> & goal);
	void						drawProductionInformation(int x, int y);
	void						setSearchGoal(MetaPairVector & goal);
};
}