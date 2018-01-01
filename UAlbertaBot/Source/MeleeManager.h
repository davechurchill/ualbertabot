#pragma once;

#include "Common.h"
#include "MicroManager.h"

namespace AKBot
{
	// Structure represents observation information
	// required for the melee units to perform actions.
	struct MeeleUnitObservation
	{
		// Observations about world captured during analysis.
		bool shouldRetreat;
		size_t meleeTargets;
		int orderDistance;

		// Execution part of melee manager frame state
		BWAPI::Position targetPosition;
		BWAPI::Unit targetUnit;
		bool shouldMove;
		bool shouldAttack;
	};
}

namespace UAlbertaBot
{

class MeleeManager : public MicroManager
{
	std::vector<BWAPI::Unit> _meleeUnitTargets;
	std::map<BWAPI::Unit, AKBot::MeeleUnitObservation> _observations;

	// Filter units which will be targets for the melee attack.
	void populateMeleeTargets(const std::vector<BWAPI::Unit> & targets);

	// Collect world representation of the melee manager.
	void collectObservations(const std::vector<BWAPI::Unit> & meleeUnits, const std::vector<BWAPI::Unit> & targets);
	
	// Generate action plan.
	void generatePlan(const std::vector<BWAPI::Unit> & meleeUnits);
	
	// Execute plan
	void executePlan(int currentFrame);
public:

	MeleeManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		const BotMicroConfiguration& microConfiguration);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	int getAttackPriority(BWAPI::Unit attacker, BWAPI::Unit unit);
	BWAPI::Unit getTarget(BWAPI::Unit meleeUnit, const std::vector<BWAPI::Unit> & targets);
    bool meleeUnitShouldRetreat(BWAPI::Unit meleeUnit, const std::vector<BWAPI::Unit> & targets);
    std::pair<BWAPI::Unit, BWAPI::Unit> findClosestUnitPair(const std::vector<BWAPI::Unit> & attackers, const std::vector<BWAPI::Unit> & targets);

    void assignTargets(const std::vector<BWAPI::Unit> & meleeUnits, const std::vector<BWAPI::Unit> & targets);
	const std::map<BWAPI::Unit, AKBot::MeeleUnitObservation>& getObservations() const;
	const std::vector<BWAPI::Unit>& getTargets() const;
};
}