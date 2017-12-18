#pragma once;

#include <Common.h>
#include "MicroManager.h"
#include "BotConfiguration.h"

namespace AKBot
{
	// Structure represents observation information
	// required for the melee units to perform actions.
	struct RangeUnitObservation
	{
		// Observations about world captured during analysis.
		bool shouldRetreat;
		bool hasRangedTargets;
		int orderDistance;

		// Execution part of melee manager frame state
		BWAPI::Position targetPosition;
		BWAPI::Unit targetUnit;
		bool shouldMove;
		bool shouldAttack;
		bool shouldMutaDance;
		bool shouldKiteTarget;
	};
}

namespace UAlbertaBot
{
class RangedManager : public MicroManager
{
	std::vector<BWAPI::Unit> _rangeUnitTargets;
	std::map<BWAPI::Unit, AKBot::RangeUnitObservation> _observations;

	// Filter units which will be targets for the range attack.
	void populateRangeTargets(const std::vector<BWAPI::Unit> & targets);

	// Collect world representation of the range manager.
	void collectObservations(const std::vector<BWAPI::Unit> & rangedUnits, const std::vector<BWAPI::Unit> & targets);

	// Generate action plan.
	void generatePlan(const std::vector<BWAPI::Unit> & rangedUnits);

	// Execute plan
	void executePlan(int currentFrame);
public:

	RangedManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		const BotMicroConfiguration& microConfiguration);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets);

    void assignTargets(const std::vector<BWAPI::Unit> & rangedUnits, const std::vector<BWAPI::Unit> & targets, int currentFrame);
};
}