#pragma once

#include <Common.h>
#include "BuildOrderQueue.h"
#include "BuildingManager.h"
#include "StrategyManager.h"
#include "BOSSManager.h"
#include "BuildOrder.h"
#include "StrategyManager.h"
#include "WorkerManager.h"

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
	shared_ptr<WorkerManager> _workerManager;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<AKBot::Logger> _logger;
    
    BWAPI::Unit         getClosestUnitToPosition(const std::vector<BWAPI::Unit> & units,BWAPI::Position closestTo);
    BWAPI::Unit         selectUnitOfType(BWAPI::UnitType type,BWAPI::Position closestTo = BWAPI::Position(0,0));

    bool                meetsReservedResources(MetaType type);
    void                setBuildOrder(const BuildOrder & buildOrder);
    void                create(BWAPI::Unit producer,BuildOrderItem & item);
    void                manageBuildOrderQueue(int currentFrame);
    void                performCommand(BWAPI::UnitCommandType t);
    bool                canMakeNow(BWAPI::Unit producer,MetaType t);
    void                predictWorkerMovement(const Building & b);

    bool                detectBuildOrderDeadlock();

    int                 getFreeMinerals();
    int                 getFreeGas();
    bool                canPlanBuildOrderNow() const;

public:
    
    ProductionManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		BOSSManager & bossManager,
		const StrategyManager& strategyManager,
		shared_ptr<WorkerManager> workerManager,
		const UnitInfoManager& unitInfo,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<MapTools> mapTools,
		std::shared_ptr<AKBot::Logger> logger);
    
    void        onStart();
    void        update(int currentFrame);
    void        onUnitDestroy(BWAPI::Unit unit, int currentFrame);
    void        performBuildOrderSearch(int currentFrame);

    BWAPI::Unit getProducer(MetaType t, int currentFrame, BWAPI::Position closestTo = BWAPI::Positions::None);
	const StrategyManager& getStrategyManager() const;
	const BuildingManager& getBuildingManager() const { return _buildingManager; }
	const BOSSManager& getBOSSManager() const { return _bossManager; }
	const shared_ptr<AKBot::OpponentView> getOpponentView() const { return _opponentView; }
	const BuildOrderQueue& getBuildOrderQueue() const { return _queue; }
};


class CompareWhenStarted
{

public:

    CompareWhenStarted() {}

    // the sorting operator
    bool operator() (BWAPI::Unit u1,BWAPI::Unit u2)
    {
        int startedU1 = u1->getType().buildTime() - u1->getRemainingBuildTime();
        int startedU2 = u2->getType().buildTime() - u2->getRemainingBuildTime();
        return startedU1 > startedU2;
    }
};
}