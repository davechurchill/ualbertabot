#include "MedicManager.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

MedicManager::MedicManager() 
{ 
}

void MedicManager::executeMicro(const BWAPI::Unitset & targets) 
{
	const BWAPI::Unitset & medics = getUnits();
    
	// create a set of all medic targets
	BWAPI::Unitset medicTargets;
    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getHitPoints() < unit->getInitialHitPoints() && !unit->getType().isMechanical() && !unit->getType().isBuilding())
        {
            medicTargets.insert(unit);
        }
    }
    
    BWAPI::Unitset availableMedics(medics);

    // for each target, send the closest medic to heal it
    for (auto & target : medicTargets)
    {
        // only one medic can heal a target at a time
        if (target->isBeingHealed())
        {
            continue;
        }

        double closestMedicDist = std::numeric_limits<double>::infinity();
        BWAPI::Unit closestMedic = nullptr;

        for (auto & medic : availableMedics)
        {
            double dist = medic->getDistance(target);

            if (!closestMedic || (dist < closestMedicDist))
            {
                closestMedic = medic;
                closestMedicDist = dist;
            }
        }

        // if we found a medic, send it to heal the target
        if (closestMedic)
        {
            closestMedic->useTech(BWAPI::TechTypes::Healing, target);

            availableMedics.erase(closestMedic);
        }
        // otherwise we didn't find a medic which means they're all in use so break
        else
        {
            break;
        }
    }

    // the remaining medics should head to the squad order position
    for (auto & medic : availableMedics)
    {
        Micro::SmartAttackMove(medic, order.getPosition());
    }
}