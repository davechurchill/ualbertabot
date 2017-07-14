#pragma once

#include <Common.h>
#include "BuildOrderQueue.h"
#include "BuildingManager.h"
#include "StrategyManager.h"
#include "BOSSManager.h"
#include "BuildOrder.h"
#include "StrategyManager.h"

namespace UAlbertaBot
{
typedef unsigned char Action;

class ProductionManager
{
    BOSSManager &       _bossManager;
    BuildingManager     _buildingManager;
    BuildOrderQueue     _queue;
    BWAPI::TilePosition _predictedTilePosition;
    bool                _enemyCloakedDetected;
    bool                _assignedWorkerForThisBuilding;
    bool                _haveLocationForThisBuilding;
	const StrategyManager& _strategyManager;
	const UnitInfoManager& _unitInfo;
    
    BWAPI::Unit         getClosestUnitToPosition(const std::vector<BWAPI::Unit> & units,BWAPI::Position closestTo);
    BWAPI::Unit         selectUnitOfType(BWAPI::UnitType type,BWAPI::Position closestTo = BWAPI::Position(0,0));

    bool                meetsReservedResources(MetaType type);
    void                setBuildOrder(const BuildOrder & buildOrder);
    void                create(BWAPI::Unit producer,BuildOrderItem & item);
    void                manageBuildOrderQueue();
    void                performCommand(BWAPI::UnitCommandType t);
    bool                canMakeNow(BWAPI::Unit producer,MetaType t);
    void                predictWorkerMovement(const Building & b);

    bool                detectBuildOrderDeadlock();

    int                 getFreeMinerals();
    int                 getFreeGas();
    bool                canPlanBuildOrderNow() const;

public:
    
    ProductionManager(BOSSManager & bossManager, const StrategyManager& strategyManager, const UnitInfoManager& unitInfo);
    
    void        onStart();
    void        update();
    void        onUnitDestroy(BWAPI::Unit unit);
    void        performBuildOrderSearch();
    void        drawProductionInformation(AKBot::ScreenCanvas& canvas, int x,int y);

    BWAPI::Unit getProducer(MetaType t,BWAPI::Position closestTo = BWAPI::Positions::None);
	const StrategyManager& getStrategyManager() const;
};


class CompareWhenStarted
{

public:

    CompareWhenStarted() {}

    // the sorting operator
    bool operator() (BWAPI::Unit u1,BWAPI::Unit u2)
    {
        int startedU1 = BWAPI::Broodwar->getFrameCount() - (u1->getType().buildTime() - u1->getRemainingBuildTime());
        int startedU2 = BWAPI::Broodwar->getFrameCount() - (u2->getType().buildTime() - u2->getRemainingBuildTime());
        return startedU1 > startedU2;
    }
};
}