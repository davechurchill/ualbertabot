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

	SparCraft::Map					map;

	UnitData							enemyUnitData;
	UnitData							selfUnitData;

	//BaseInfoVector						allBases;
	BWTA::BaseLocation *				mainBaseLocations[2];
	std::set<BWTA::Region *>			occupiedRegions[2];

	int									getIndex(BWAPI::Player * player);

	void								updateUnit(BWAPI::Unit * unit);
	void								initializeRegionInformation();
	void								initializeBaseInfoVector();
	void								updateUnitInfo();
	void								updateBaseInfo();
	void								updateBaseLocationInfo();
	void								updateOccupiedRegions(BWTA::Region * region, BWAPI::Player * player);
	bool								isValidUnit(BWAPI::Unit * unit);

	const UnitData &					getUnitData(BWAPI::Player * player) const;
	const UnitData &					getUnitData(BWAPI::Unit * unit) const;

public:

	bool goForIt;
	int lastFrameRegroup;

	// yay for singletons!
	static InformationManager &			Instance();

	void								update();
	void								onStart();

	// event driven stuff
	void					onUnitShow(BWAPI::Unit * unit)			{ updateUnit(unit); }
	void					onUnitHide(BWAPI::Unit * unit)			{ updateUnit(unit); }
	void					onUnitCreate(BWAPI::Unit * unit)		{ updateUnit(unit); }
	void					onUnitMorph(BWAPI::Unit * unit)			{ updateUnit(unit); }
	void					onUnitRenegade(BWAPI::Unit * unit)		{ updateUnit(unit); }
	void					onUnitDestroy(BWAPI::Unit * unit);

	bool					positionInRangeOfEnemyDetector(BWAPI::Position p);
	bool					enemyFlyerThreat();
	bool					isEnemyBuildingInRegion(BWTA::Region * region);
	int						getNumUnits(BWAPI::UnitType type, BWAPI::Player * player);
	int						getNumTotalDeadUnits(BWAPI::Player * player);
	int						numEnemyUnitsInRegion(BWTA::Region * region);
	int						numEnemyFlyingUnitsInRegion(BWTA::Region * region);
	bool					nearbyForceHasCloaked(BWAPI::Position p, BWAPI::Player * player, int radius);
	bool					isCombatUnit(BWAPI::UnitType type) const;
	bool					canWinNearby(BWAPI::Position p);
	bool					tileContainsUnit(BWAPI::TilePosition tile);

	void					getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::Player * player, int radius);

	std::pair<double, double>	nearbyCombatInfo(BWAPI::Position p, BWAPI::Player * player);
	double						getDPS(BWAPI::UnitType type);

	UnitInfoVector &			getEnemyDetectors();
	UnitInfoVector &			getKnownUnitInfo(BWAPI::UnitType type, BWAPI::Player * player);
	const UIMap &				getUnitInfo(BWAPI::Player * player) const;

	BWAPI::Unit *				getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);

	std::set<BWTA::Region *> &	getOccupiedRegions(BWAPI::Player * player);
	BWTA::BaseLocation *		getMainBaseLocation(BWAPI::Player * player);

	bool						enemyHasCloakedUnits();
	bool						enemyHasDetector();

	void						drawUnitInformation(int x, int y);

	SparCraft::Map *			getMap() { return &map; }
};
}


/*
struct BaseInfo {

	BWTA::BaseLocation *	baseLocation;
	int						lastFrameSeen;

	BaseInfo (BWTA::BaseLocation * base) {

		baseLocation = base;
	}

	bool isExplored() { return BWAPI::Broodwar->isExplored(baseLocation->getTilePosition()); }
	bool isVisible()  {	return BWAPI::Broodwar->isVisible(baseLocation->getTilePosition());  }

	bool isBuiltOn() {

		const UnitInfoVector & ourUnits = InformationManager::Instance().getUnitInfo(BWAPI::Broodwar->self());
		const UnitInfoVector & enemyUnits = InformationManager::Instance().getUnitInfo(BWAPI::Broodwar->self());

		int radius = 400;
		
		// check each of our units
		for (size_t i(0); i<ourUnits.size(); ++i) {

			if (ourUnits[i].type.isBuilding() && ourUnits[i].lastPosition.getDistance(baseLocation->getPosition()) < radius) {

				return true;
			}
		}

		// check each of the enemy units
		for (size_t i(0); i<enemyUnits.size(); ++i) {

			if (enemyUnits[i].type.isBuilding() && enemyUnits[i].lastPosition.getDistance(baseLocation->getPosition()) < radius) {

				return true;
			}
		}

		return false;
	}

};
*/