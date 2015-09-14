#include "ReplayVisualizer.h"

#ifdef USING_VISUALIZATION_LIBRARIES

/*
ReplayVisualizer::ReplayVisualizer() 
	: map(BWAPI::Broodwar)
{
	
}

void ReplayVisualizer::launchSimulation(const BWAPI::Position & center, const int & radius)
{
	// set up the display object
	SparCraft::Display display(SparCraft::Display(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight()));
	display.OnStart();
	display.LoadMapTexture(&map, 19);

	// extract the state from the current state of BWAPI
	SparCraft::GameState state;
	setCombatUnits(state, center, radius);
	state.setMap(&map);

	// get search player objects for us and the opponent
	PlayerPtr selfPlayer(getSearchPlayer(SparCraft::PlayerToMove::Alternate, SparCraft::Players::Player_One, SparCraft::EvaluationMethods::Playout, 40));
	PlayerPtr enemyPlayer(SparCraft::AllPlayers::getPlayerPtr(SparCraft::Players::Player_Two, SparCraft::PlayerModels::AttackClosest));

	// set up the game
	SparCraft::Game g(state, selfPlayer, enemyPlayer, 1000);
	g.disp = &display;

	// play the game to the end
	g.play();
}

void ReplayVisualizer::setCombatUnits(SparCraft::GameState & state, const BWAPI::Position & center, const int radius)
{

	int selfUnits = 0;
	BOOST_FOREACH (BWAPI::UnitInterface* unit, BWAPI::Broodwar->getPlayer(1)->getUnits())
	{
		bool inRadius = unit->getDistance(center) < radius;

		if (selfUnits < 8 && inRadius && isCombatUnit(unit->getType()))
		{
			selfUnits++;
			// FIX state.addUnit(SparCraft::Unit(unit, SparCraft::Players::Player_One, BWAPI::Broodwar->getFrameCount()));
		}
		else
		{
			// FIX state.addNeutralUnit(SparCraft::Unit(unit, SparCraft::Players::Player_One, BWAPI::Broodwar->getFrameCount()));
		}
	}

	int enemyUnits = 0;
	BOOST_FOREACH (BWAPI::UnitInterface* unit, BWAPI::Broodwar->getPlayer(0)->getUnits())
	{
		if (enemyUnits >= 8)
		{
			break;
		}

		bool inRadius = unit->getDistance(center) < radius;

		if (enemyUnits < 8 && inRadius && isCombatUnit(unit->getType()) && !unit->getType().isFlyer())
		{
			enemyUnits++;
			// FIX state.addUnit(SparCraft::Unit(unit,Search::Players::Player_Two, BWAPI::Broodwar->getFrameCount()));
		}
		else
		{
			// FIX state.addNeutralUnit(SparCraft::Unit(unit, Search::Players::Player_Two, BWAPI::Broodwar->getFrameCount()));
		}
	}

	int neutralUnits = 0;
	BOOST_FOREACH (BWAPI::UnitInterface* unit, BWAPI::Broodwar->getAllUnits())
	{
		neutralUnits++;

		const IDType player(getPlayer(unit->getPlayer()));

		if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field ||
			unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
		{
			// FIX state.addNeutralUnit(SparCraft::Unit(unit, Search::Players::Player_None, BWAPI::Broodwar->getFrameCount()));
		}
	}

	state.finishedMoving();
}

bool ReplayVisualizer::isCombatUnit(BWAPI::UnitType type) const
{
	if (type == BWAPI::UnitTypes::Zerg_Lurker || type == BWAPI::UnitTypes::Protoss_Dark_Templar)
	{
		return false;
	}

	// no workers or buildings allowed
	if (type.isWorker())
	{
		return false;
	}

	// check for various types of combat units
	if (type.canAttack() || type == BWAPI::UnitTypes::Terran_Medic)
	{
		return true;
	}
		
	return false;
}

PlayerPtr ReplayVisualizer::getSearchPlayer(const IDType & playerToMoveMethod, const IDType & playerID, const IDType & evalMethod, const size_t & timeLimitMS)
{
    IDType bestResponseTo = SparCraft::PlayerModels::NOKDPS;

	// base parameters to use in search
	SparCraft::AlphaBetaSearchParameters baseParameters;
	baseParameters.setMaxPlayer(playerID);
	baseParameters.setSearchMethod(SparCraft::SearchMethods::IDAlphaBeta);
	baseParameters.setEvalMethod(evalMethod);
	baseParameters.setMaxDepth(SparCraft::Constants::Max_Search_Depth);
    //baseParameters.setScriptMoveFirstMethod(SparCraft::PlayerModels::NOKDPS);
	baseParameters.setTimeLimit(timeLimitMS);
	
	// IF USING OPPONENT MODELING SET IT HERE
	baseParameters.setModelSimMethod(bestResponseTo);
	baseParameters.setPlayerModel(Search::Players::Player_Two, bestResponseTo, true);

	baseParameters.setPlayerToMoveMethod(playerToMoveMethod);
	return PlayerPtr(new MicroSearch::Player_AlphaBeta(playerID, baseParameters, MicroSearch::TTPtr(new MicroSearch::TranspositionTable())));
}

const MicroSearch::Unit ReplayVisualizer::getUnit(const UnitInfo & ui, const IDType & playerID) const
{
	BWAPI::UnitType type = ui.type;

	return MicroSearch::Unit(ui.type, MicroSearch::Position(ui.lastPosition.x, ui.lastPosition.y), ui.unitID, playerID, ui.lastHealth, 0,
		BWAPI::Broodwar->getFrameCount(), BWAPI::Broodwar->getFrameCount());	
}

const IDType ReplayVisualizer::getPlayer(BWAPI::UnitInterface* unit) const
{
	return getPlayer(unit->getPlayer());
}

const IDType ReplayVisualizer::getPlayer(BWAPI::PlayerInterface * player) const
{
	if (player == BWAPI::Broodwar->self())
	{
		return SparCraft::Players::Player_Two;
	}
	else if (player == BWAPI::Broodwar->enemy())
	{
		return SparCraft::Players::Player_One;
	}

	return SparCraft::Players::Player_None;
}
*/
#endif