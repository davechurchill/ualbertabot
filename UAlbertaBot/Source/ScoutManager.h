#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class ScoutManager 
{
	BWAPI::UnitInterface*	workerScout;
    std::string             scoutStatus;
    std::string             gasStealStatus;
	int				        numWorkerScouts;
	bool			        scoutUnderAttack;
    bool                    didGasSteal;
	
	bool                    enemyWorkerInRadius();
    bool			        immediateThreat();
    void                    gasSteal();
	BWAPI::UnitInterface*	getEnemyGeyser();
	BWAPI::UnitInterface*	closestEnemyWorker();

	void moveScouts();
    void drawScoutInformation(int x, int y);

public:

	ScoutManager();
	~ScoutManager() {};

	void update(const std::set<BWAPI::UnitInterface*> & scoutUnits);

	void onSendText(std::string text);
	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void onUnitDestroy(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
};
}