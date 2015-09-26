#pragma once

#include "Common.h"
#include "BWTA.h"

#include "UnitData.h"

#include "..\..\SparCraft\source\SparCraft.h"

namespace UAlbertaBot
{
struct BaseInfo;
typedef std::vector<BaseInfo> BaseInfoVector;

class InformationManager {

	InformationManager();

	SparCraft::Map					    map;

	UnitData							enemyUnitData;
	UnitData							selfUnitData;

	//BaseInfoVector					allBases;
    BWAPI::TilePosition                 mainBaseTilePositions[2];
	BWTA::BaseLocation *				mainBaseLocations[2];
	std::set<BWTA::Region *>			occupiedRegions[2];
    std::set<BWAPI::Position>			occupiedBases[2];

	int									getIndex(BWAPI::PlayerInterface * player);

	void								updateUnit(BWAPI::Unit unit);
	void								initializeRegionInformation();
	void								initializeBaseInfoVector();
	void								updateUnitInfo();
	void								updateBaseInfo();
	void								updateBaseLocationInfo();
	void								updateOccupiedRegions(BWTA::Region * region, BWAPI::PlayerInterface * player);
	bool								isValidUnit(BWAPI::Unit unit);

public:

	// yay for singletons!
	static InformationManager &			Instance();

	void								update();
	void								onStart();

	// event driven stuff
	void					onUnitShow(BWAPI::Unit unit)			{ updateUnit(unit); }
	void					onUnitHide(BWAPI::Unit unit)			{ updateUnit(unit); }
	void					onUnitCreate(BWAPI::Unit unit)		{ updateUnit(unit); }
	void					onUnitComplete(BWAPI::Unit unit)		{ updateUnit(unit); }
	void					onUnitMorph(BWAPI::Unit unit)			{ updateUnit(unit); }
	void					onUnitRenegade(BWAPI::Unit unit)		{ updateUnit(unit); }
	void					onUnitDestroy(BWAPI::Unit unit);

	bool					positionInRangeOfEnemyDetector(BWAPI::Position p);
	bool					enemyFlyerThreat();
	bool					isEnemyBuildingInRegion(BWTA::Region * region);
	int						getNumUnits(BWAPI::UnitType type, BWAPI::PlayerInterface * player);
	int						getNumTotalDeadUnits(BWAPI::PlayerInterface * player);
	int						numEnemyUnitsInRegion(BWTA::Region * region);
	int						numEnemyFlyingUnitsInRegion(BWTA::Region * region);
	bool					nearbyForceHasCloaked(BWAPI::Position p, BWAPI::PlayerInterface * player, int radius);
	bool					isCombatUnit(BWAPI::UnitType type) const;
	bool					canWinNearby(BWAPI::Position p);
	bool					tileContainsUnit(BWAPI::TilePosition tile);

	void					getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::PlayerInterface * player, int radius);

	std::pair<double, double>	nearbyCombatInfo(BWAPI::Position p, BWAPI::PlayerInterface * player);
	double						getDPS(BWAPI::UnitType type);

	UnitInfoVector &			getEnemyDetectors();
	UnitInfoVector &			getKnownUnitInfo(BWAPI::UnitType type, BWAPI::PlayerInterface * player);
	const UIMap &				getUnitInfo(BWAPI::PlayerInterface * player) const;

	BWAPI::Unit				getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);

	std::set<BWTA::Region *> &	getOccupiedRegions(BWAPI::PlayerInterface * player);
	BWTA::BaseLocation *		getMainBaseLocation(BWAPI::PlayerInterface * player);

	bool						enemyHasCloakedUnits();
	bool						enemyHasDetector();

    void						drawExtendedInterface();
	void						drawUnitInformation(int x, int y);
	void						drawMapInformation();
	SparCraft::Map *			getMap() { return &map; }

	const UnitData &					getUnitData(BWAPI::PlayerInterface * player) const;
	const UnitData &					getUnitData(BWAPI::Unit unit) const;
};
}
