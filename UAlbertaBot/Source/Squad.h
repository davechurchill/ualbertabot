#pragma once

#include "Common.h"
#include "MeleeManager.h"
#include "RangedManager.h"
#include "DetectorManager.h"
#include "TransportManager.h"
#include "SquadOrder.h"
#include "DistanceMap.hpp"
#include "StrategyManager.h"
#include "CombatSimulation.h"


namespace UAlbertaBot
{
class ZealotManager;
class DarkTemplarManager;
class DragoonManager;
class ObserverManager;

class MeleeManager;
class RangedManager;
class DetectorManager;

class Squad
{
	std::vector<BWAPI::UnitInterface *>			units;
	std::string			regroupStatus;
	bool				squadObserverNear(BWAPI::Position p);
	
	SquadOrder			order;
	MeleeManager		meleeManager;
	RangedManager		rangedManager;
	DetectorManager		detectorManager;
	TransportManager	transportManager;

	std::map<BWAPI::UnitInterface*, bool>	nearEnemy;

	void				updateUnits();
	void				addUnitsToMicroManagers();
	void				setNearEnemyUnits();
	void				setAllUnits();
	
	void				setUnits(const std::vector<BWAPI::UnitInterface *> & u)	{ units = u; }
	
	bool				unitNearEnemy(BWAPI::UnitInterface* unit);
	bool				needsToRegroup();
	BWAPI::UnitInterface*		getRegroupUnit();
	int					squadUnitsNear(BWAPI::Position p);

	BWAPI::UnitInterface*		unitClosestToEnemy();

    static int          lastRetreatSwitch;
    static bool         lastRetreatSwitchVal;

protected:
	void				addUnit(BWAPI::UnitInterface *u);
	bool				removeUnit(BWAPI::UnitInterface *u);
public:


	Squad(const std::vector<BWAPI::UnitInterface *> & units, SquadOrder order);
	Squad() {}
	~Squad() {}

	BWAPI::Position		calcCenter();
	BWAPI::Position		calcRegroupPosition();

	void				update();

	const std::vector<BWAPI::UnitInterface *> &	getUnits() const;
	const SquadOrder &	getSquadOrder()	const;

	void				setSquadOrder(const SquadOrder & so);
};
}