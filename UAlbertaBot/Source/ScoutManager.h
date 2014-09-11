#pragma once

#include "Common.h"
#include "micromanagement/MicroManager.h"
#include "micromanagement/MicroUtil.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class ScoutManager {

	BWAPI::Unit *	workerScout;
	int				numWorkerScouts;

	bool			immediateThreat();

	BWAPI::Unit *	closestEnemyWorker();

	bool			scoutUnderAttack;
	
	void smartMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition);
	void smartAttack(BWAPI::Unit * attacker, BWAPI::Unit * target);
	bool enemyWorkerInRadius();

	BWAPI::Position		calcFleePosition(const std::vector<GroundThreat> & threats, BWAPI::Unit * target);
	bool				isValidFleePosition(BWAPI::Position pos);
	void				fillGroundThreats(std::vector<GroundThreat> & threats, BWAPI::Position target);
	double2				getFleeVector(const std::vector<GroundThreat> & threats);
	BWAPI::Unit *		getEnemyGeyser();


public:

	ScoutManager();
	~ScoutManager() {};

	void update(const std::set<BWAPI::Unit *> & scoutUnits);
	void moveScouts();

	void onSendText(std::string text);
	void onUnitShow(BWAPI::Unit * unit);
	void onUnitHide(BWAPI::Unit * unit);
	void onUnitCreate(BWAPI::Unit * unit);
	void onUnitRenegade(BWAPI::Unit * unit);
	void onUnitDestroy(BWAPI::Unit * unit);
	void onUnitMorph(BWAPI::Unit * unit);
};
}