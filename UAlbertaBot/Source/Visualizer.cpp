#include "Visualizer.h"

#ifdef USING_VISUALIZATION_LIBRARIES

Visualizer & Visualizer::Instance() 
{
	static Visualizer instance;
	return instance;
}

Visualizer::Visualizer() 
	: display(SparCraft::Display(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight()))
{
	map = SparCraft::Map(BWAPI::Broodwar);
	map.write("C:\\test.txt");
	display.OnStart();
	
	display.LoadMapTexture(&map, 19);
}

void Visualizer::setBWAPIState()
{
	SparCraft::GameState state;
	state.setMap(&map);

	BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getAllUnits())
	{
		const IDType player(getPlayer(unit->getPlayer()));

		if (player == SparCraft::Players::Player_One || player == SparCraft::Players::Player_Two)
		{
			// FIX state.addUnit(SparCraft::Unit(unit, player, BWAPI::Broodwar->getFrameCount()));
		}
		else
		{
			if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field ||
				unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
			{
				// FIX state.addNeutralUnit(SparCraft::Unit(unit, SparCraft::Players::Player_None, BWAPI::Broodwar->getFrameCount()));
			}
		}
	}

	setState(state);
}

void Visualizer::setState(const SparCraft::GameState & state)
{
	display.SetState(state);
}

void Visualizer::onFrame()
{
	display.OnFrame();
}

const IDType Visualizer::getPlayer(BWAPI::Unit * unit) const
{
	return getPlayer(unit->getPlayer());
}

const IDType Visualizer::getPlayer(BWAPI::Player * player) const
{
	if (player == BWAPI::Broodwar->self())
	{
		return SparCraft::Players::Player_One;
	}
	else if (player == BWAPI::Broodwar->enemy())
	{
		return SparCraft::Players::Player_Two;
	}

	return SparCraft::Players::Player_None;
}

#endif