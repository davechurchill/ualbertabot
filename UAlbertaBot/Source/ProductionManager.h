#pragma once

#include <Common.h>
#include "BuildOrderQueue.h"

namespace UAlbertaBot
{
typedef unsigned char Action;

class Building;
class BuildOrder;

class ProductionManager
{
    friend class Global;

    ProductionManager();

    BuildOrderQueue     m_queue;
    BWAPI::TilePosition m_predictedTilePosition;
    bool                m_enemyCloakedDetected          = false;
    bool                m_assignedWorkerForThisBuilding = false;
    bool                m_haveLocationForThisBuilding   = false;

    BWAPI::Unit         getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closestTo);
    BWAPI::Unit         selectUnitOfType(BWAPI::UnitType type, BWAPI::Position closestTo = BWAPI::Position(0, 0));

    void setBuildOrder(const BuildOrder & buildOrder);
    void create(BWAPI::Unit producer, BuildOrderItem & item);
    void manageBuildOrderQueue();
    void performCommand(BWAPI::UnitCommandType t);
    void predictWorkerMovement(const Building & b);

    int  getFreeMinerals();
    int  getFreeGas();
    bool detectBuildOrderDeadlock();
    bool canPlanBuildOrderNow() const;
    bool canMakeNow(BWAPI::Unit producer, MetaType t);
    bool meetsReservedResources(MetaType type);

public:

    void update();
    void onUnitDestroy(BWAPI::Unit unit);
    void performBuildOrderSearch();
    void drawProductionInformation(int x, int y);
    void queueGasSteal();

    BWAPI::Unit getProducer(MetaType t, BWAPI::Position closestTo = BWAPI::Positions::None);
};


class CompareWhenStarted
{

public:

    CompareWhenStarted() {}

    // the sorting operator
    bool operator() (BWAPI::Unit u1, BWAPI::Unit u2)
    {
        int startedU1 = BWAPI::Broodwar->getFrameCount() - (u1->getType().buildTime() - u1->getRemainingBuildTime());
        int startedU2 = BWAPI::Broodwar->getFrameCount() - (u2->getType().buildTime() - u2->getRemainingBuildTime());
        return startedU1 > startedU2;
    }
};
}