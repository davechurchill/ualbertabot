#include "MedicManager.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/TechType.h>
#include <BWAPI/Unitset.h>
#include "Micro.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;
using UAlbertaBot::UnitUtil::GetClosestsUnitToTarget;

MedicManager::MedicManager(shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<BaseLocationManager> bases)
	: MicroManager(opponentView, bases)
{
}

void MedicManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	// create a set of all medic targets
	std::vector<BWAPI::Unit> medicTargets;
    for (auto & unit : opponentView->self()->getUnits())
    {
        if (unit->getHitPoints() < unit->getInitialHitPoints() && !unit->getType().isMechanical() && !unit->getType().isBuilding())
        {
            medicTargets.push_back(unit);
        }
    }
    
	const std::vector<BWAPI::Unit> & medics = getUnits();

	std::vector<BWAPI::Unit> availableMedics(medics);

    // for each target, send the closest medic to heal it
    for (auto & target : medicTargets)
    {
        // only one medic can heal a target at a time
        if (target->isBeingHealed())
        {
            continue;
        }

        auto closestMedic = GetClosestsUnitToTarget(availableMedics, target);

		// if we didn't find a medic which means they're all in use so break
		if (!closestMedic) {
			break;
		}

        // if we found a medic, send it to heal the target
		closestMedic->useTech(BWAPI::TechTypes::Healing, target);
		availableMedics.erase(std::remove(availableMedics.begin(), availableMedics.end(), closestMedic), availableMedics.end());
    }

    // the remaining medics should head to the squad order position
    for (auto & medic : availableMedics)
    {
        Micro::SmartAttackMove(medic, order.getPosition(), currentFrame);
    }
}