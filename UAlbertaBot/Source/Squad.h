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
#include "TankManager.h"
#include "MedicManager.h"

namespace UAlbertaBot
{
    
class Squad
{
    std::string         _name;
	BWAPI::Unitset      _units;
	std::string         _regroupStatus;
    int                 _lastRetreatSwitch;
    bool                _lastRetreatSwitchVal;
    size_t              _priority;
	
	SquadOrder          _order;
	MeleeManager        _meleeManager;
	RangedManager       _rangedManager;
	DetectorManager     _detectorManager;
	TransportManager    _transportManager;
    TankManager         _tankManager;
    MedicManager        _medicManager;

	std::map<BWAPI::Unit, bool>	_nearEnemy;

    
	BWAPI::Unit		getRegroupUnit();
	BWAPI::Unit		unitClosestToEnemy();
    
	void                        updateUnits();
	void                        addUnitsToMicroManagers();
	void                        setNearEnemyUnits();
	void                        setAllUnits();
	
	bool                        unitNearEnemy(BWAPI::Unit unit);
	bool                        needsToRegroup();
	int                         squadUnitsNear(BWAPI::Position p);

public:

	Squad(const std::string & name, SquadOrder order, size_t priority);
	Squad();
    ~Squad();

	void                update();
	void                setSquadOrder(const SquadOrder & so);
	void                addUnit(BWAPI::Unit u);
	void                removeUnit(BWAPI::Unit u);
    bool                containsUnit(BWAPI::Unit u) const;
    bool                isEmpty() const;
    void                clear();
    size_t              getPriority() const;
    void                setPriority(const size_t & priority);
    const std::string & getName() const;
    
	BWAPI::Position     calcCenter();
	BWAPI::Position     calcRegroupPosition();

	const BWAPI::Unitset &  getUnits() const;
	const SquadOrder &  getSquadOrder()	const;
};
}