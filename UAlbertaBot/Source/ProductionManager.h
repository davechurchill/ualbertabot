#pragma once

#include "Common.h"
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
	shared_ptr<BOSSManager>       _bossManager;
	shared_ptr<BuildingManager>     _buildingManager;
    BuildOrderQueue     _queue;
    BWAPI::TilePosition _predictedTilePosition;
    bool                _enemyCloakedDetected;
	bool                _enemyCloakedDetectedThisFrame;
	bool				_emptyQueueDetected;
	bool                _queueDeadlockDetected;
    bool                _assignedWorkerForThisBuilding;
    bool                _haveLocationForThisBuilding;
	shared_ptr<StrategyManager> _strategyManager;
	shared_ptr<UnitInfoManager> _unitInfo;
	shared_ptr<WorkerManager> _workerManager;
	shared_ptr<AKBot::OpponentView> _opponentView;
	bool				_useBuildOrderSearch;
	const BotDebugConfiguration& _debugConfiguration;
    
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
		shared_ptr<BOSSManager> bossManager,
		shared_ptr<BuildingManager> buildingManager,
		shared_ptr<StrategyManager> strategyManager,
		shared_ptr<WorkerManager> workerManager,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<MapTools> mapTools,
		const BotDebugConfiguration& debugConfiguration);
	ProductionManager(const ProductionManager&) = delete;
    
    void        onStart();
    void        update(int currentFrame);
    void        onUnitDestroy(BWAPI::Unit unit, int currentFrame);
    void        performBuildOrderSearch(int currentFrame);

    BWAPI::Unit getProducer(MetaType t, int currentFrame, BWAPI::Position closestTo = BWAPI::Positions::None);
	const shared_ptr<StrategyManager> getStrategyManager() const;
	const shared_ptr<BuildingManager> getBuildingManager() const { return _buildingManager; }
	const shared_ptr<BOSSManager> getBOSSManager() const { return _bossManager; }
	const shared_ptr<AKBot::OpponentView> getOpponentView() const { return _opponentView; }
	const BuildOrderQueue& getBuildOrderQueue() const { return _queue; }
	const bool getEmptyQueueDetected() const { return _emptyQueueDetected; }
	const bool getQueueDeadlockDetected() const { return _queueDeadlockDetected; }
	const bool getCloackedUnitDetectedThisFrame() const { return _enemyCloakedDetectedThisFrame; }
	const bool getUseBuildOrderSearch() const { return _useBuildOrderSearch; }
	void setUseBuildOrderSearch(bool value) { _useBuildOrderSearch = value; }
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