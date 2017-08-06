#pragma once

#include "Common.h"
#include "MeleeManager.h"
#include "RangedManager.h"
#include "DetectorManager.h"
#include "TransportManager.h"
#include "SquadOrder.h"
#include "StrategyManager.h"
#include "CombatSimulation.h"
#include "TankManager.h"
#include "MedicManager.h"
#include "UnitHandler.h"
#include "MapTools.h"
#include "OpponentView.h"
#include "Logger.h"
#include "MapTools.h"

namespace UAlbertaBot
{
	using namespace AKBot;
    
class Squad
{
    std::string         _name;
	std::set<BWAPI::Unit> _units;
	std::string         _regroupStatus;
    int                 _lastRetreatSwitch;
    bool                _lastRetreatSwitchVal;
    size_t              _priority;
	UnitHandler			_onRemoveHandler;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<AKBot::Logger> _logger;
	shared_ptr<UnitInfoManager> _unitInfo;
	
	SquadOrder          _order;
	MeleeManager        _meleeManager;
	RangedManager       _rangedManager;
	DetectorManager     _detectorManager;
	TransportManager    _transportManager;
    TankManager         _tankManager;
    MedicManager        _medicManager;

	std::map<BWAPI::Unit, bool>	_nearEnemy;
	BWAPI::Position _lastRegroupPosition;
	bool			_needToRegroup;

	BWAPI::Unit		unitClosestToEnemy(std::function<int(const BWAPI::Position & src, const BWAPI::Position & dest)> distance);
	void                        updateUnits(shared_ptr<MapTools> map);
	void                        addUnitsToMicroManagers();
	void                        setNearEnemyUnits(shared_ptr<MapTools> map);
	void                        setAllUnits();
	
	bool                        unitNearEnemy(shared_ptr<MapTools> map, BWAPI::Unit unit);
	bool                        needsToRegroup(shared_ptr<MapTools> map, int currentFrame);
	int                         squadUnitsNear(BWAPI::Position p);

public:

	Squad(
		const std::string & name,
		SquadOrder order,
		size_t priority,
		AKBot::PlayerLocationProvider& locationProvider,
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<MapTools> mapTools,
		std::shared_ptr<AKBot::Logger> logger);
    ~Squad();

	void                update(shared_ptr<MapTools> map, int currentFrame);
	void                setSquadOrder(const SquadOrder & so);
	void                addUnit(BWAPI::Unit u);
	void                removeUnit(BWAPI::Unit u);
    bool                containsUnit(BWAPI::Unit u) const;
    bool                isEmpty() const;
    void                clear(int currentFrame);
    size_t              getPriority() const;
    void                setPriority(const size_t & priority);

	// Get name of the squad.
    const std::string & getName() const;
	const std::string& getRegroupStatus() const;
	bool getNeedToRegroup() const;
	const MeleeManager getMeleeManager() const { return _meleeManager; }
	const RangedManager getRangedManager() const { return _rangedManager; }
	const TankManager getTankManager() const { return _tankManager; }
	const MedicManager getMedicManager() const { return _medicManager; }
	const DetectorManager getDetectorManager() const { return _detectorManager; }
	const TransportManager getTransportManager() const { return _transportManager; }

	/*
	 Calculate center of the squad.
	*/
	BWAPI::Position     calcCenter();
	BWAPI::Position     calcRegroupPosition();

	const std::set<BWAPI::Unit> &  getUnits() const;
	const SquadOrder &  getSquadOrder()	const;
	void onUnitRemoved(UnitHandler handler);
	bool isNearEnemy(const BWAPI::Unit& unit) const;

	BWAPI::Position getLastRegroupPosition() const { return _lastRegroupPosition; };
};
}