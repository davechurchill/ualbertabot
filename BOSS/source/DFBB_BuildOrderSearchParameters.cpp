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



std::string DFBB_BuildOrderSearchParameters::toString() const
{
    std::stringstream ss;

    ss << "\n\nSearch Parameter Information\n\n";

    ss << (useRepetitions ?                    "\tUSE      Repetitions\n" : "");
    ss << (useIncreasingRepetitions ?          "\tUSE      Increasing Repetitions\n" : "");
    ss << (useLandmarkLowerBoundHeuristic ?    "\tUSE      Landmark Lower Bound\n" : "");
    ss << (useResourceLowerBoundHeuristic ?    "\tUSE      Resource Lower Bound\n" : "");
    ss << (useAlwaysMakeWorkers ?              "\tUSE      Always Make Workers\n" : "");
    ss << (useSupplyBounding ?                 "\tUSE      Supply Bounding\n" : "");
    ss << ("\n");

    for (ActionID a(0); a < repetitionValues.size(); ++a)
    {
        if (repetitionValues[a] != 1)
        {
            ss << "\tREP " << repetitionValues[a] << " " << ActionTypes::GetActionType(race, a).getName() << "\n";
        }
    }

    for (ActionID a(0); a < repetitionThresholds.size(); ++a)
    {
        if (repetitionThresholds[a] != 0)
        {
            ss << "\tTHR " << repetitionThresholds[a] << " " << ActionTypes::GetActionType(race, a).getName() << "\n";
        }
    }

    ss << "\n\n" << goal.toString();
    ss << "\n\n" << initialState.toString();

    for (size_t i(0); i < relevantActions.size(); ++i)
    {
        ss << "Relevant:   " << relevantActions[i].getName() << "\n";
    }

    ss << "\n\n" << initialState.getUnitData().getBuildingData().toString();

    return ss.str();
}