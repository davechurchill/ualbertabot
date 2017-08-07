#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "UnitHandler.h"
#include "BaseLocationManager.h"

namespace UAlbertaBot
{
	using namespace AKBot;

class ScoutManager 
{
	shared_ptr<BaseLocationManager> _baseLocationManager;
	BWAPI::Unit     _workerScout;
    std::string     _scoutStatus;
	int             _numWorkerScouts;
	bool            _scoutUnderAttack;
    int             _currentRegionVertexIndex;
    int             _previousScoutHP;
	std::vector<BWAPI::Position>    _enemyRegionVertices;
	UnitHandler     _onScoutReleased;
	UnitHandler     _onScoutAssigned;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<MapTools> _mapTools;

	bool            enemyWorkerInRadius();
    bool            immediateThreat();
    int             getClosestVertexIndex(BWAPI::Unit unit);
    BWAPI::Position getFleePosition();
	BWAPI::Unit     getEnemyGeyser();
	BWAPI::Unit     closestEnemyWorker();
    //void            followPerimeter();
	void            moveScouts(int currentFrame);
	bool exploreEnemyBases(int currentFrame);
	bool allEnemyBasesExplored() const;
	void harrasEnemyBaseIfPossible(const BaseLocation * enemyBaseLocation, int scoutHP, int currentFrame);
    //void            calculateEnemyRegionVertices();


public:
    
	ScoutManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<MapTools> mapTools);

	void update(int currentFrame);
    void setWorkerScout(BWAPI::Unit unit, int currentFrame);
	void onScoutReleased(UnitHandler handler);
	void onScoutAssigned(UnitHandler handler);
	const std::string getScoutStatus() const { return _scoutStatus; }
	const std::vector<BWAPI::Position>& getEnemyRegionVerticies() const { return _enemyRegionVertices; }
	const BWAPI::Unit& getWorkerScount() const { return _workerScout; }
};
}