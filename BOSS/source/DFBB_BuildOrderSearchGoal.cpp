#include "DFBB_BuildOrderSearchGoal.h"

using namespace BOSS;

DFBB_BuildOrderSearchGoal::DFBB_BuildOrderSearchGoal(const RaceID race)
    : _supplyRequiredVal(0)
    , _goalUnits(race != Races::None ? ActionTypes::GetAllActionTypes(race).size() : 0, 0)
    , _goalUnitsMax(race != Races::None ? ActionTypes::GetAllActionTypes(race).size() : 0, 0)
    , _race(race)
{
 
}

void DFBB_BuildOrderSearchGoal::calculateSupplyRequired()
{
    _supplyRequiredVal = 0;
    for (ActionID a(0); a<_goalUnits.size(); ++a)
    {
        _supplyRequiredVal += _goalUnits[a] * ActionTypes::GetActionType(_race, a).supplyRequired();
    }
}

bool DFBB_BuildOrderSearchGoal::operator == (const DFBB_BuildOrderSearchGoal & g)
{
    for (ActionID a(0); a<_goalUnits.size(); ++a)
    {
        if ((_goalUnits[a] != g._goalUnits[a]) || (_goalUnitsMax[a] != g._goalUnitsMax[a]))
        {
            return false;
        }
    }

    return true;
}

void DFBB_BuildOrderSearchGoal::setGoal(const ActionType & a, const UnitCountType num)
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnits.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    _goalUnits[a.ID()] = num;

    calculateSupplyRequired();
}

bool DFBB_BuildOrderSearchGoal::hasGoal() const
{
    for (ActionID a(0); a<_goalUnits.size(); ++a)
    {
        if (_goalUnits[a] > 0)
        {
            return true;
        }
    }

    return false;
}

void DFBB_BuildOrderSearchGoal::setGoalMax(const ActionType & a, const UnitCountType num)
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnitsMax.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    _goalUnitsMax[a.ID()] = num;
}

UnitCountType DFBB_BuildOrderSearchGoal::getGoal(const ActionType & a) const
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnits.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    return _goalUnits[a.ID()];
}

UnitCountType DFBB_BuildOrderSearchGoal::getGoalMax(const ActionType & a) const
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnitsMax.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    return _goalUnitsMax[a.ID()];
}

SupplyCountType DFBB_BuildOrderSearchGoal::supplyRequired() const
{
    return _supplyRequiredVal;
}

void DFBB_BuildOrderSearchGoal::printGoal() const
{
    printf("\nSearch Goal Information\n\n");

    for (ActionID a(0); a<_goalUnits.size(); ++a)
    {
        if (_goalUnits[a] > 0)
        {
            printf("        REQ %7d %s\n",_goalUnits[a], ActionTypes::GetActionType(_race, a).getName().c_str());
        }
    }

    for (ActionID a(0); a<_goalUnitsMax.size(); ++a)
    {
        if (_goalUnitsMax[a] > 0)
        {
            printf("        MAX %7d %s\n", _goalUnitsMax[a], ActionTypes::GetActionType(_race, a).getName().c_str());
        }
    }
}
