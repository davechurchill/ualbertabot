#pragma once

#include "Common.h"
#include "MicroManager.h"

namespace UAlbertaBot
{
class ScoutManager 
{
	BWAPI::Unit     _workerScout;
    std::string     _scoutStatus;
	int             _numWorkerScouts;
	bool            _scoutUnderAttack;
    int             _currentRegionVertexIndex;
    int             _previousScoutHP;
	std::vector<BWAPI::Position>    _enemyRegionVertices;

	bool            enemyWorkerInRadius();
    bool            immediateThreat();
    int             getClosestVertexIndex(BWAPI::Unit unit);
    BWAPI::Position getFleePosition();
	BWAPI::Unit     getEnemyGeyser();
	BWAPI::Unit     closestEnemyWorker();
    //void            followPerimeter();
	void            moveScouts();
    void            drawScoutInformation(int x, int y);
    //void            calculateEnemyRegionVertices();


public:
    
	ScoutManager();

	void update();
    void setWorkerScout(BWAPI::Unit unit);

};
}