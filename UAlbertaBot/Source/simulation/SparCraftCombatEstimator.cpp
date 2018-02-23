#include "SparCraftCombatEstimator.h"
#include "UnitUtil.h"

using namespace AKBot;
using namespace UAlbertaBot;

AKBot::SparCraftCombatEstimator::SparCraftCombatEstimator(
	shared_ptr<AKBot::OpponentView> opponentView,
	std::shared_ptr<AKBot::Logger> logger,
	const BotSparCraftConfiguration& sparcraftConfiguration,
	const BotMicroConfiguration& microConfiguration)
	: _opponentView(opponentView)
	, _logger(logger)
	, _sparcraftConfiguration(sparcraftConfiguration)
	, _microConfiguration(microConfiguration)
{
}

bool AKBot::SparCraftCombatEstimator::isWinPredicted(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	setCombatUnits(ourCombatUnits, enemyCombatUnits, currentFrame);
	auto score = simulateCombat();
	return score > _microConfiguration.RetreatThreshold;
}


// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
void AKBot::SparCraftCombatEstimator::setCombatUnits(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	SparCraft::GameState s;

	for (auto & unit : ourCombatUnits)
	{
		if (unit->getType().isWorker() || unit->getHitPoints() == 0 || unit->getType().isBuilding())
		{
			continue;
		}

		if (UnitUtil::IsCombatUnit(unit) && SparCraft::System::UnitTypeSupported(unit->getType()))
		{
			try
			{
				s.addUnit(getSparCraftUnit(unit, currentFrame));
			}
			catch (int e)
			{
				e = 1;
				_logger->log("Problem Adding Self Unit with ID: %d", unit->getID());
			}
		}
	}

	for (UnitInfo & ui : enemyCombatUnits)
	{
		if (ui.type.isWorker() || ui.lastHealth == 0 || ui.type == BWAPI::UnitTypes::Unknown)
		{
			continue;
		}

		// if it's a bunker that has a nontrivial amount of hit points, replace it by 5 marines
		if (ui.type == BWAPI::UnitTypes::Terran_Bunker && ui.lastHealth > 10)
		{
			double hpRatio = static_cast<double>(ui.lastHealth) / ui.type.maxHitPoints();

			SparCraft::Unit marine(BWAPI::UnitTypes::Terran_Marine,
				SparCraft::Position(ui.lastPosition),
				ui.unitID,
				getSparCraftPlayerID(ui.player),
				static_cast<SparCraft::HealthType>(BWAPI::UnitTypes::Terran_Marine.maxHitPoints() * hpRatio),
				static_cast<SparCraft::HealthType>(0),
				static_cast<SparCraft::TimeType>(currentFrame),
				static_cast<SparCraft::TimeType>(currentFrame));

			for (size_t i(0); i < 5; ++i)
			{
				s.addUnit(marine);
			}

			continue;
		}

		if (!ui.type.isFlyer() && SparCraft::System::UnitTypeSupported(ui.type) && ui.completed)
		{
			try
			{
				s.addUnit(getSparCraftUnit(ui, currentFrame));
			}
			catch (int e)
			{
				_logger->log("Problem Adding Enemy Unit with ID: %d %d", ui.unitID, e);
			}
		}
	}

	_state = s;
}

// Gets a SparCraft unit from a BWAPI::Unit, used for our own units since we have all their info
const SparCraft::Unit AKBot::SparCraftCombatEstimator::getSparCraftUnit(BWAPI::Unit unit, int currentFrame) const
{
	return SparCraft::Unit(unit->getType(),
		SparCraft::Position(unit->getPosition()),
		unit->getID(),
		getSparCraftPlayerID(unit->getPlayer()),
		static_cast<SparCraft::HealthType>(unit->getHitPoints() + unit->getShields()),
		static_cast<SparCraft::HealthType>(0),
		static_cast<SparCraft::TimeType>(currentFrame),
		static_cast<SparCraft::TimeType>(currentFrame));
}

// Gets a SparCraft unit from a UnitInfo struct, needed to get units of enemy behind FoW
const SparCraft::Unit SparCraftCombatEstimator::getSparCraftUnit(const UnitInfo & ui, int currentFrame) const
{
	BWAPI::UnitType type = ui.type;

	// this is a hack, treat medics as a marine for now
	if (type == BWAPI::UnitTypes::Terran_Medic)
	{
		type = BWAPI::UnitTypes::Terran_Marine;
	}

	return SparCraft::Unit(ui.type,
		SparCraft::Position(ui.lastPosition),
		ui.unitID,
		getSparCraftPlayerID(ui.player),
		static_cast<SparCraft::HealthType>(ui.lastHealth),
		static_cast<SparCraft::TimeType>(0),
		static_cast<SparCraft::TimeType>(currentFrame),
		static_cast<SparCraft::TimeType>(currentFrame));
}

double AKBot::SparCraftCombatEstimator::simulateCombat()
{
	try
	{
		SparCraft::GameState originalState(_state);
		size_t selfID = getSparCraftPlayerID(_opponentView->self());
		size_t enemyID = getSparCraftPlayerID(_opponentView->defaultEnemy());

		SparCraft::PlayerPtr selfNOK(new SparCraft::Player_AttackClosest(selfID));
		SparCraft::PlayerPtr enemyNOK(new SparCraft::Player_AttackClosest(enemyID));

		auto& aiParameters = SparCraft::AIParameters::Instance();
		SparCraft::PlayerPtr p1 = aiParameters.getPlayer(selfID, _sparcraftConfiguration.CombatSimPlayerName);
		SparCraft::PlayerPtr p2 = aiParameters.getPlayer(enemyID, _sparcraftConfiguration.CombatSimPlayerName);

		SparCraft::Game game(originalState, p1, p2, 2000);

		game.play();

		_evaluatedState = game.getState();
		_lastScore = SparCraft::Eval::Eval(_evaluatedState, SparCraft::Players::Player_One, SparCraft::EvaluationMethods::LTD2).val();
		//std::cout << "LTD2: " << SparCraft::Eval::LTD2(g.getState(), 0) << ", " << SparCraft::Eval::LTD2(g.getState(), 1) << "\n";

		return _lastScore;
	}
	catch (int e)
	{
		_logger->log("SparCraft FatalError, simulateCombat() threw");

		return e;
	}
}

const SparCraft::GameState & AKBot::SparCraftCombatEstimator::getSparCraftState() const
{
	return _state;
}

const size_t AKBot::SparCraftCombatEstimator::getSparCraftPlayerID(BWAPI::Player player) const
{
	if (player == _opponentView->self())
	{
		return SparCraft::Players::Player_One;
	}
	else if (player == _opponentView->defaultEnemy())
	{
		return SparCraft::Players::Player_Two;
	}

	return SparCraft::Players::Player_None;
}