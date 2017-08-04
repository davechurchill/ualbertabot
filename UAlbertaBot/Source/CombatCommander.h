#pragma once

#include "Common.h"
#include "Squad.h"
#include "SquadData.h"
#include "BaseLocationManager.h"
#include "DefaultPlayerLocationProvider.h"
#include "OpponentView.h"

namespace UAlbertaBot
{
class CombatCommander
{
	SquadData       _squadData;
    std::vector<BWAPI::Unit>  _combatUnits;
    bool            _initialized;
	const BaseLocationManager & _baseLocationManager;
	AKBot::DefaultPlayerLocationProvider _playerLocationProvider;
	const AKBot::OpponentView& _opponentView;
	const UnitInfoManager& _unitInfo;
	const MapTools& _mapTools;

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
    BWAPI::Position getMainAttackLocation();

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

public:

	CombatCommander(
		const BaseLocationManager & baseLocationManager,
		const AKBot::OpponentView& opponentView,
		const UnitInfoManager& unitInfo,
		const MapTools& mapTools,
		const AKBot::Logger& logger);

	void update(const std::vector<BWAPI::Unit> & combatUnits, int currentFrame);
	const SquadData& getSquadData() const;
};
}
