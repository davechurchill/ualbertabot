#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "ProductionManager.h"
#include "ScoutManager.h"
#include "Timer.hpp"
#include "ScreenCanvas.h"

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

    std::vector<BWAPI::Unit>    _validUnits;
    std::vector<BWAPI::Unit>    _combatUnits;
    std::vector<BWAPI::Unit>    _scoutUnits;

    bool                        _initialScoutSet;

    void assignUnit(BWAPI::Unit unit,std::vector<BWAPI::Unit> & set);
    bool isAssigned(BWAPI::Unit unit) const;

public:

    GameCommander(UAlbertaBot_Tournament & uabModule);

    void onStart();
    void update();

    void handleUnitAssignments();
    void setValidUnits();
    void setScoutUnits();
    void setCombatUnits();

    void drawDebugInterface(AKBot::ScreenCanvas& canvas);
    void drawGameInformation(AKBot::ScreenCanvas& canvas, int x,int y);

    BWAPI::Unit getFirstSupplyProvider();
    BWAPI::Unit getClosestUnitToTarget(BWAPI::UnitType type,BWAPI::Position target);
    BWAPI::Unit getClosestWorkerToTarget(BWAPI::Position target);

    ProductionManager & getProductionManager();

    void onUnitShow(BWAPI::Unit unit);
    void onUnitHide(BWAPI::Unit unit);
    void onUnitCreate(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onUnitRenegade(BWAPI::Unit unit);
    void onUnitDestroy(BWAPI::Unit unit);
    void onUnitMorph(BWAPI::Unit unit);
};

}