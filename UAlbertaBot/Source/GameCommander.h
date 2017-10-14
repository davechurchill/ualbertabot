#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "ProductionManager.h"
#include "ScoutManager.h"
#include "Timer.hpp"
#include "BWAPIOpponentView.h"

namespace UAlbertaBot
{

class GameCommander
{
    Timer                       _timer;
	shared_ptr<CombatCommander>            _combatCommander;
	shared_ptr<BOSSManager>                _bossManager;
	shared_ptr<ProductionManager>          _productionManager;
	shared_ptr<ScoutManager>	_scoutManager;
	shared_ptr<WorkerManager> _workerManager;
	shared_ptr<AKBot::OpponentView>	_opponentView;

    std::vector<BWAPI::Unit>    _validUnits;
    std::vector<BWAPI::Unit>    _combatUnits;
    std::vector<BWAPI::Unit>    _scoutUnits;

    bool                        _initialScoutSet;

    void assignUnit(BWAPI::Unit unit,std::vector<BWAPI::Unit> & set);
    bool isAssigned(BWAPI::Unit unit) const;

public:

    GameCommander(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BOSSManager> bossManager,
		shared_ptr<CombatCommander> combatCommander,
		shared_ptr<ScoutManager> scoutManager,
		shared_ptr<ProductionManager> productionManager,
		shared_ptr<WorkerManager> workerManager);

    void onStart();
    void update(int currentFrame);

    void handleUnitAssignments(int currentFrame);
    void setValidUnits();
    void setScoutUnits(int currentFrame);
    void setCombatUnits();
	std::vector<BWAPI::Unit>& getCombatUnits() { return _combatUnits; }

	BWAPI::Unit getFirstSupplyProvider();
    BWAPI::Unit getClosestUnitToTarget(BWAPI::UnitType type,BWAPI::Position target);
    BWAPI::Unit getClosestWorkerToTarget(BWAPI::Position target);

	const shared_ptr<ProductionManager> getProductionManager() const;
	const shared_ptr<ScoutManager> getScoutManager() const;
	const shared_ptr<CombatCommander> getCombatCommander() const;
	const shared_ptr<AKBot::OpponentView> getOpponentView() const;

    void onUnitShow(BWAPI::Unit unit);
    void onUnitHide(BWAPI::Unit unit);
    void onUnitCreate(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onUnitRenegade(BWAPI::Unit unit);
    void onUnitDestroy(BWAPI::Unit unit, int currentFrame);
    void onUnitMorph(BWAPI::Unit unit);
};

}