#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class ScoutManager 
{
	BWAPI::UnitInterface*	        _workerScout;
    std::string                     _scoutStatus;
    std::string                     _gasStealStatus;
	int				                _numWorkerScouts;
	bool			                _scoutUnderAttack;
    bool                            _didGasSteal;
    bool                            _gasStealFinished;
    int                             _currentRegionVertexIndex;
    int                             _previousScoutHP;
	std::vector<BWAPI::Position>    _enemyRegionVertices;

	bool                            enemyWorkerInRadius();
    bool			                immediateThreat();
    void                            gasSteal();
    int                             getClosestVertexIndex(BWAPI::UnitInterface * unit);
    BWAPI::Position                 getFleePosition();
	BWAPI::UnitInterface*	        getEnemyGeyser();
	BWAPI::UnitInterface*	        closestEnemyWorker();
    void                            followPerimeter();
	void                            moveScouts();
    void                            drawScoutInformation(int x, int y);
    void                            calculateEnemyRegionVertices();

	ScoutManager();

public:

    static ScoutManager & Instance();

	void update();

    void setWorkerScout(BWAPI::UnitInterface * unit);

	void onSendText(std::string text);
	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void onUnitDestroy(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
};
}