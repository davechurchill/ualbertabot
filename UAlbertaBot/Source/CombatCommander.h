#pragma once

#include "Common.h"
#include "Squad.h"
#include "SquadData.h"
#include "BaseLocationManager.h"
#include "DefaultPlayerLocationProvider.h"
#include "OpponentView.h"
#include "WorkerManager.h"

namespace UAlbertaBot
{
class CombatCommander
{
	SquadData       _squadData;
    std::vector<BWAPI::Unit>  _combatUnits;
    bool            _initialized;
	shared_ptr<BaseLocationManager> _baseLocationManager;
	AKBot::DefaultPlayerLocationProvider _playerLocationProvider;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<UnitInfoManager> _unitInfo;
	shared_ptr<MapTools> _mapTools;
	shared_ptr<WorkerManager> _workerManager;

    void            updateScoutDefenseSquad(int currentFrame);
	void            updateDefenseSquads(int currentFrame);
	void            updateAttackSquads();
    void            updateDropSquads();
	void            updateIdleSquad();
	bool            isSquadUpdateFrame(int currentFrame) const;
	int             getNumType(std::vector<BWAPI::Unit> & units, BWAPI::UnitType type);

	BWAPI::Unit     findClosestDefender(const Squad & defenseSquad, BWAPI::Position pos, bool flyingDefender, int currentFrame);
    BWAPI::Unit     findClosestWorkerToTarget(std::vector<BWAPI::Unit> & unitsToAssign, BWAPI::Unit target);

	BWAPI::Position getDefendLocation();
    BWAPI::Position getMainAttackLocation(BWAPI::Position squadPosition);

    void            initializeSquads();
    void            verifySquadUniqueMembership();
    void            assignFlyingDefender(Squad & squad);
    void            emptySquad(Squad & squad, std::vector<BWAPI::Unit> & unitsToAssign);
    int             getNumGroundDefendersInSquad(Squad & squad);
    int             getNumAirDefendersInSquad(Squad & squad);

    void            updateDefenseSquadUnits(
		Squad & defenseSquad,
		const size_t & flyingDefendersNeeded,
		const size_t & groundDefendersNeeded,
		int currentFrame);
    int             defendWithWorkers();

    int             numZerglingsInOurBase();
    bool            beingBuildingRushed();
	bool findEnemyBaseLocation(BWAPI::Position& basePosition);
	bool findEnemyBuilding(BWAPI::Position& buildingPosition);
	// Find enemy unit which is closest to the given position.
	bool findEnemyUnit(BWAPI::Position targetPosition, BWAPI::Position& unitPosition);
public:

	CombatCommander(
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<WorkerManager> workerManager,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<MapTools> mapTools,
		shared_ptr<AKBot::Logger> logger);

	void update(const std::vector<BWAPI::Unit> & combatUnits, int currentFrame);
	const SquadData& getSquadData() const;
};
}
