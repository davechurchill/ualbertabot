#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "MicroUtil.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class ScoutManager {

	BWAPI::UnitInterface*	workerScout;
	int				numWorkerScouts;

	bool			immediateThreat();

	BWAPI::UnitInterface*	closestEnemyWorker();

	bool			scoutUnderAttack;
	
	void smartMove(BWAPI::UnitInterface* attacker, BWAPI::Position targetPosition);
	void smartAttack(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target);
	bool enemyWorkerInRadius();

	BWAPI::Position		calcFleePosition(const std::vector<GroundThreat> & threats, BWAPI::UnitInterface* target);
	bool				isValidFleePosition(BWAPI::Position pos);
	void				fillGroundThreats(std::vector<GroundThreat> & threats, BWAPI::Position target);
	double2				getFleeVector(const std::vector<GroundThreat> & threats);
	BWAPI::UnitInterface*		getEnemyGeyser();


public:

	ScoutManager();
	~ScoutManager() {};

	void update(const std::set<BWAPI::UnitInterface*> & scoutUnits);
	void moveScouts();

	void onSendText(std::string text);
	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void onUnitDestroy(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
};
}