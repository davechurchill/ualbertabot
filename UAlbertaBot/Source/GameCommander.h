#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "ProductionManager.h"
#include "ScoutManager.h"
#include "Timer.hpp"
#include "BWAPIOpponentView.h"

namespace UAlbertaBot
{

class UAlbertaBot_Tournament;

class GameCommander
{
    Timer                       _timer;
    CombatCommander             _combatCommander;
    BOSSManager                 _bossManager;
    ProductionManager           _productionManager;
    ScoutManager                _scoutManager;
    UAlbertaBot_Tournament &     _module;
	const AKBot::OpponentView&	_opponentView;

    std::vector<BWAPI::Unit>    _validUnits;
    std::vector<BWAPI::Unit>    _combatUnits;
    std::vector<BWAPI::Unit>    _scoutUnits;

    bool                        _initialScoutSet;

    void assignUnit(BWAPI::Unit unit,std::vector<BWAPI::Unit> & set);
    bool isAssigned(BWAPI::Unit unit) const;

public:

    GameCommander(
		UAlbertaBot_Tournament & uabModule,
		const AKBot::OpponentView& opponentView,
		const BaseLocationManager& bases,
		const AKBot::Logger& logger);

    void onStart();
    void update(int currentFrame);

    void handleUnitAssignments(int currentFrame);
    void setValidUnits();
    void setScoutUnits(int currentFrame);
    void setCombatUnits();

	BWAPI::Unit getFirstSupplyProvider();
    BWAPI::Unit getClosestUnitToTarget(BWAPI::UnitType type,BWAPI::Position target);
    BWAPI::Unit getClosestWorkerToTarget(BWAPI::Position target);

	const ProductionManager& getProductionManager() const;
	const ScoutManager& getScoutManager() const;
	const CombatCommander& getCombatCommander() const;
	const AKBot::OpponentView& getOpponentView() const;

    void onUnitShow(BWAPI::Unit unit);
    void onUnitHide(BWAPI::Unit unit);
    void onUnitCreate(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onUnitRenegade(BWAPI::Unit unit);
    void onUnitDestroy(BWAPI::Unit unit, int currentFrame);
    void onUnitMorph(BWAPI::Unit unit);
};

}