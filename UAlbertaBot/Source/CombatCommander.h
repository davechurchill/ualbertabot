#pragma once
#include <experimental/generator>
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
	bool			_supportDropStrategy;
	const BotMicroConfiguration& _microConfiguration;
	int totalGroundUnitsNearBases;
	int totalFlyingUnitsNearBases;
	size_t totalGroundDefendersAdded;
	size_t totalFlyingDefendersAdded;
	size_t totalGroundDefendersNeeded;
	size_t totalFlyingDefendersNeeded;

	std::experimental::generator<const BaseLocation *> getBasesToProtect();
    void            updateScoutDefenseSquad(int currentFrame);
	void            updateDefenseSquads(int currentFrame);
	void            updateAttackSquads();
	void updateAttackSquad(UAlbertaBot::Squad & mainAttackSquad, size_t maxUnitsInSquad);
    void            updateDropSquads();
	void            updateIdleSquad();
	bool            isSquadUpdateFrame(int currentFrame) const;
	int             getNumType(std::vector<BWAPI::Unit> & units, BWAPI::UnitType type);

	BWAPI::Unit     findClosestWorkerToTarget(std::vector<BWAPI::Unit> & unitsToAssign, BWAPI::Unit target);

	BWAPI::Position getDefendLocation();
    BWAPI::Position getMainAttackLocation(BWAPI::Position squadPosition);

    void            initializeSquads();
    void            verifySquadUniqueMembership();
    void            assignFlyingDefender(Squad & squad);
    void            emptySquad(Squad & squad, std::vector<BWAPI::Unit> & unitsToAssign);
    int             getNumGroundDefendersInSquad(Squad & squad);
    int             getNumAirDefendersInSquad(Squad & squad);
    int             defendWithWorkers();

    int             numUnitsInOurBase(BWAPI::UnitType unitType, BWAPI::Position basePosition, int radius);
    bool            beingBuildingRushed(BWAPI::Position basePosition);
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
		shared_ptr<AKBot::Logger> logger,
		const BotMicroConfiguration& microConfiguration,
		const BotSparCraftConfiguration& sparcraftConfiguration,
		const BotDebugConfiguration& debugConfiguration);
	CombatCommander(const CombatCommander&) = delete;

	void update(const std::vector<BWAPI::Unit> & combatUnits, int currentFrame);
	const SquadData& getSquadData() const;
	bool getSupportsDropSquad() const { return _supportDropStrategy; }
	void setSupportsDropSquad(bool value) { _supportDropStrategy = value; }
	void setRushModeEnabled(bool value) { _squadData.setRushModeEnabled(value); }
	void setRushUnitType(BWAPI::UnitType rushUnit) { _squadData.setRushUnitType(rushUnit); }
	void setAllowedRushUnitLoses(int value) { _squadData.setAllowedRushUnitLoses(value); }

	int getTotalGroundUnitsNearBases() const { return totalGroundUnitsNearBases; }
	int getTotalFlyingUnitsNearBases() const { return totalFlyingUnitsNearBases; }
	int getTotalGroundDefendersAdded() const { return totalGroundDefendersAdded; }
	int getTotalFlyingDefendersAdded() const { return totalFlyingDefendersAdded; }
	int getTotalGroundDefendersNeeded() const { return totalGroundDefendersNeeded; }
	int getTotalFlyingDefendersNeeded() const { return totalFlyingDefendersNeeded; }
};
}
