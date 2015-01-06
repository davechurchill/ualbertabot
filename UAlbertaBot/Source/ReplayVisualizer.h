#pragma once

#include "..\..\SparCraft\source\SparCraft.h"

#ifdef USING_VISUALIZATION_LIBRARIES

#include <Common.h>

#include "InformationManager.h"
#include "MapGrid.h"

namespace UAlbertaBot
{
class ReplayVisualizer 
{
	SparCraft::Map map;

	const IDType getPlayer(BWAPI::UnitInterface* unit) const;
	const IDType getPlayer(BWAPI::PlayerInterface * player) const;
	void setCombatUnits(SparCraft::GameState & s, const BWAPI::Position & center, const int radius);
	const SparCraft::Unit getUnit(const UnitInfo & ui, const IDType & playerID) const;
	bool isCombatUnit(BWAPI::UnitType type) const;

	SparCraft::PlayerPtr getSearchPlayer(const IDType & playerToMoveMethod, const IDType & playerID, const IDType & evalMethod, const size_t & timeLimitMS);

public:

	ReplayVisualizer();
	void ReplayVisualizer::launchSimulation(const BWAPI::Position & pos, const int & radius);
};
}

#endif