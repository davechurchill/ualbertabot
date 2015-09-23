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

	std::map<BWAPI::UnitInterface*, bool>	_nearEnemy;

    
	BWAPI::UnitInterface*		getRegroupUnit();
	BWAPI::UnitInterface*		unitClosestToEnemy();
    
	void                        updateUnits();
	void                        addUnitsToMicroManagers();
	void                        setNearEnemyUnits();
	void                        setAllUnits();
	
	bool                        unitNearEnemy(BWAPI::UnitInterface* unit);
	bool                        needsToRegroup();
	int                         squadUnitsNear(BWAPI::Position p);

public:

	Squad(const std::string & name, SquadOrder order, size_t priority);
	Squad();
    ~Squad();

	void                update();
	void                setSquadOrder(const SquadOrder & so);
	void                addUnit(BWAPI::UnitInterface * u);
	void                removeUnit(BWAPI::UnitInterface * u);
    bool                containsUnit(BWAPI::UnitInterface * u) const;
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