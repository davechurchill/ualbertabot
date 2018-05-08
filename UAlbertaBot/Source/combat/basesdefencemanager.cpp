#include "BasesDefenceManager.h"
#include "BaseDefenceManager.h"
#include "CombatPriorities.h"

using namespace UAlbertaBot;
using namespace AKBot;

AKBot::BasesDefenceManager::BasesDefenceManager(
	UAlbertaBot::SquadData & squadData,
	std::vector<const BaseLocation *>& basesToProtect,
	const std::vector<BWAPI::Unit> & combatUnits,
	UnitCollection& enemyUnits,
	const BotMicroConfiguration& microConfiguration)
	: _squadData(squadData)
	, _combatUnits(combatUnits)
	, _enemyUnits(enemyUnits)
	, _microConfiguration(microConfiguration)
	, _basesToProtect(basesToProtect)
{
}

void AKBot::BasesDefenceManager::update(int currentFrame)
{
	totalGroundUnitsNearBases = 0;
	totalFlyingUnitsNearBases = 0;
	totalGroundDefendersAdded = 0;
	totalFlyingDefendersAdded = 0;
	totalGroundDefendersNeeded = 0;
	totalFlyingDefendersNeeded = 0;

	for (auto myBaseLocation : _basesToProtect)
	{
        BaseDefenceManager baseDefence(_squadData, myBaseLocation, _combatUnits, _enemyUnits, _microConfiguration);
	    baseDefence.update(currentFrame);

        totalGroundUnitsNearBases += baseDefence.getTotalGroundUnitsNearBases();
        totalFlyingUnitsNearBases += baseDefence.getTotalFlyingUnitsNearBases();
        totalGroundDefendersAdded += baseDefence.getTotalGroundDefendersAdded();
        totalFlyingDefendersAdded += baseDefence.getTotalFlyingDefendersAdded();
        totalGroundDefendersNeeded += baseDefence.getTotalGroundDefendersNeeded();
        totalFlyingDefendersNeeded += baseDefence.getTotalFlyingDefendersNeeded();
    }

	// for each of our defense squads, if there aren't any enemy units near the position, remove the squad
	std::set<std::string> uselessDefenseSquads;
	for (const auto & kv : _squadData.getSquads())
	{
		const Squad & squad = kv.second;
		const SquadOrder & order = squad.getSquadOrder();

		if (order.getType() != SquadOrderTypes::Defend)
		{
			continue;
		}

		bool enemyUnitInRange = false;
		for (auto & unit : _enemyUnits)
		{
			if (unit->getPosition().getDistance(order.getPosition()) < order.getRadius())
			{
				enemyUnitInRange = true;
				break;
			}
		}

		if (!enemyUnitInRange)
		{
			_squadData.getSquad(squad.getName()).clear(currentFrame);
		}
	}
}