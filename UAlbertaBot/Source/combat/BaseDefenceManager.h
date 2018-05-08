#pragma once
#include <vector>
#include "../SquadData.h"
#include "../BotConfiguration.h"
#include "../UnitUtil.h"

namespace AKBot
{
	class BaseDefenceManager
	{
		UAlbertaBot::SquadData& _squadData;
		const std::vector<BWAPI::Unit>& _combatUnits;
		UnitCollection& _enemyUnits;
		const BotMicroConfiguration& _microConfiguration;
		const BaseLocation * _baseToProtect;

		int totalGroundUnitsNearBases;
		int totalFlyingUnitsNearBases;
		size_t totalGroundDefendersAdded;
		size_t totalFlyingDefendersAdded;
		size_t totalGroundDefendersNeeded;
		size_t totalFlyingDefendersNeeded;

		BWAPI::Unit findClosestDefender(
			BWAPI::Position basePosition,
			const UAlbertaBot::Squad & defenseSquad,
			BWAPI::Position pos,
			bool flyingDefender,
			int currentFrame);
		void updateDefenseSquadUnits(
			BWAPI::Position basePosition,
			UAlbertaBot::Squad & defenseSquad,
			const size_t & flyingDefendersNeeded,
			const size_t & groundDefendersNeeded,
			int currentFrame);
		bool beingBuildingRushed(BWAPI::Position basePosition);
		int numUnitsInOurBase(BWAPI::UnitType unitType, BWAPI::Position basePosition, int radius);
	public:
		BaseDefenceManager(
			UAlbertaBot::SquadData& squadData,
			const BaseLocation * baseToProtect,
			const std::vector<BWAPI::Unit> & combatUnits,
			UnitCollection& enemyUnits,
			const BotMicroConfiguration& microConfiguration);
		void update(int currentFrame);

		// Get defence radius for the base.
		int getBaseDefenceRadius() const { return 600; }

		int getTotalGroundUnitsNearBases() const { return totalGroundUnitsNearBases; }
		int getTotalFlyingUnitsNearBases() const { return totalFlyingUnitsNearBases; }
		int getTotalGroundDefendersAdded() const { return totalGroundDefendersAdded; }
		int getTotalFlyingDefendersAdded() const { return totalFlyingDefendersAdded; }
		int getTotalGroundDefendersNeeded() const { return totalGroundDefendersNeeded; }
		int getTotalFlyingDefendersNeeded() const { return totalFlyingDefendersNeeded; }

		int getFlyingEnemiesInRegion(UnitCollection enemyUnitsInRegion) const;
		int getGroundEnemiesInRegion(UnitCollection enemyUnitsInRegion) const;
		int getFlyingDefendersNeeded(UnitCollection enemyUnitsInRegion) const;
		int getGroundDefendersNeeded(UnitCollection enemyUnitsInRegion) const;
		bool isDefenceRequired(UnitCollection enemyUnitsInRegion, int currentFrame) const;
		UnitCollection getEnemyUnitsInRegion() const;

		static std::string getBaseDefenceSquadName(BWAPI::Position basePosition)
		{
			std::stringstream squadName;
			squadName << "Base Defense " << basePosition.x << " " << basePosition.y;
			return squadName.str();
		}
	};
}
