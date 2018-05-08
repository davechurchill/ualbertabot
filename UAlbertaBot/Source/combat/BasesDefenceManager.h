#pragma once
#include <vector>
#include <experimental/generator>
#include "../SquadData.h"
#include "../BotConfiguration.h"
#include "../UnitUtil.h"

namespace AKBot
{
	class BasesDefenceManager
	{
		UAlbertaBot::SquadData& _squadData;
		const std::vector<BWAPI::Unit>& _combatUnits;
		UnitCollection& _enemyUnits;
		const BotMicroConfiguration& _microConfiguration;
		std::vector<const BaseLocation *> _basesToProtect;

		int totalGroundUnitsNearBases;
		int totalFlyingUnitsNearBases;
		size_t totalGroundDefendersAdded;
		size_t totalFlyingDefendersAdded;
		size_t totalGroundDefendersNeeded;
		size_t totalFlyingDefendersNeeded;
	public:
		BasesDefenceManager(
			UAlbertaBot::SquadData& squadData,
			std::vector<const BaseLocation *>& basesToProtect,
			const UnitCollection& combatUnits,
			UnitCollection& enemyUnits,
			const BotMicroConfiguration& microConfiguration);
		void update(int currentFrame);

		int getTotalGroundUnitsNearBases() const { return totalGroundUnitsNearBases; }
		int getTotalFlyingUnitsNearBases() const { return totalFlyingUnitsNearBases; }
		int getTotalGroundDefendersAdded() const { return totalGroundDefendersAdded; }
		int getTotalFlyingDefendersAdded() const { return totalFlyingDefendersAdded; }
		int getTotalGroundDefendersNeeded() const { return totalGroundDefendersNeeded; }
		int getTotalFlyingDefendersNeeded() const { return totalFlyingDefendersNeeded; }

		static std::string getBaseDefenceSquadName(BWAPI::Position basePosition)
		{
			std::stringstream squadName;
			squadName << "Base Defense " << basePosition.x << " " << basePosition.y;
			return squadName.str();
		}
	};
}
