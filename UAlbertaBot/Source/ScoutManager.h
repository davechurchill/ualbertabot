#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "UnitHandler.h"

namespace UAlbertaBot
{
	using namespace AKBot;
	class BaseLocation;
	class BaseLocationManager;

class ScoutManager 
{
	const BaseLocationManager& _baseLocationManager;
	BWAPI::Unit     _workerScout;
    std::string     _scoutStatus;
	int             _numWorkerScouts;
	bool            _scoutUnderAttack;
    int             _currentRegionVertexIndex;
    int             _previousScoutHP;
	std::vector<BWAPI::Position>    _enemyRegionVertices;
	UnitHandler     _onScoutReleased;
	UnitHandler     _onScoutAssigned;

	bool            enemyWorkerInRadius();
    bool            immediateThreat();
    int             getClosestVertexIndex(BWAPI::Unit unit);
    BWAPI::Position getFleePosition();
	BWAPI::Unit     getEnemyGeyser();
	BWAPI::Unit     closestEnemyWorker();
    //void            followPerimeter();
	void            moveScouts();
	bool exploreEnemyBases();
	bool allEnemyBasesExplored() const;
	void harrasEnemyBaseIfPossible(const BaseLocation * enemyBaseLocation, int scoutHP);
    //void            calculateEnemyRegionVertices();


public:
    
	ScoutManager(const BaseLocationManager& baseLocationManager);

	void update();
    void setWorkerScout(BWAPI::Unit unit);
	void drawScoutInformation(AKBot::ScreenCanvas& canvas, int x, int y);
	void onScoutReleased(UnitHandler handler);
	void onScoutAssigned(UnitHandler handler);
};
}