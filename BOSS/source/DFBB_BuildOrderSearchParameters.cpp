#include "DFBB_BuildOrderSearchParameters.h"

using namespace BOSS;

// alternate constructor
DFBB_BuildOrderSearchParameters::DFBB_BuildOrderSearchParameters(const RaceID & r)
    : race(r)
    , useRepetitions(true)
    , useIncreasingRepetitions(false)
    , useAlwaysMakeWorkers(false)
    , useSupplyBounding(false)
    , supplyBoundingThreshold(1)
    , useLandmarkLowerBoundHeuristic(true)
    , useResourceLowerBoundHeuristic(true)
    , searchTimeLimit(0)
    , initialUpperBound(0)
    , repetitionValues(Constants::MAX_ACTIONS, 1)
    , repetitionThresholds(Constants::MAX_ACTIONS, 0)
    , goal(r)
{
    
}

void DFBB_BuildOrderSearchParameters::setRepetitions(const ActionType & a, const UnitCountType & repetitions)
{ 
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < repetitionValues.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == race, "Action type race doesn't match this parameter object");

    repetitionValues[a.ID()] = repetitions; 
}

void DFBB_BuildOrderSearchParameters::setRepetitionThreshold(const ActionType & a, const UnitCountType & thresh)	
{ 
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < repetitionThresholds.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == race, "Action type race doesn't match this parameter object");

    repetitionThresholds[a.ID()] = thresh; 
}

const UnitCountType & DFBB_BuildOrderSearchParameters::getRepetitions(const ActionType & a)
{ 
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < repetitionValues.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == race, "Action type race doesn't match this parameter object");

    return repetitionValues[a.ID()]; 
}

const UnitCountType & DFBB_BuildOrderSearchParameters::getRepetitionThreshold(const ActionType & a)				
{ 
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < repetitionThresholds.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == race, "Action type race doesn't match this parameter object");

    return repetitionThresholds[a.ID()]; 
}

void DFBB_BuildOrderSearchParameters::print()
{
    printf("\n\nSearch Parameter Information\n\n");

    printf("%s", useRepetitions ?                    "\tUSE      Repetitions\n" : "");
    printf("%s", useIncreasingRepetitions ?          "\tUSE      Increasing Repetitions\n" : "");
    printf("%s", useLandmarkLowerBoundHeuristic ?    "\tUSE      Landmark Lower Bound\n" : "");
    printf("%s", useResourceLowerBoundHeuristic ?    "\tUSE      Resource Lower Bound\n" : "");
    printf("%s", useAlwaysMakeWorkers ?              "\tUSE      Always Make Workers\n" : "");
    printf("%s", useSupplyBounding ?                 "\tUSE      Supply Bounding\n" : "");
    printf("\n");

    for (ActionID a(0); a < repetitionValues.size(); ++a)
    {
        if (repetitionValues[a] != 1)
        {
            printf("\tREP %7d %s\n", repetitionValues[a], ActionTypes::GetActionType(race, a).getName().c_str());
        }
    }

    for (ActionID a(0); a < repetitionThresholds.size(); ++a)
    {
        if (repetitionThresholds[a] != 0)
        {
            printf("\tTHR %7d %s\n", repetitionThresholds[a], ActionTypes::GetActionType(race, a).getName().c_str());
        }
    }

    printf("\n\n");
}