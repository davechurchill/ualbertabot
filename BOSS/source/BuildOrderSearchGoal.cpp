#include "BuildOrderSearchGoal.h"

using namespace BOSS;

BuildOrderSearchGoal::BuildOrderSearchGoal(const RaceID race)
    : _supplyRequiredVal(0)
    , _goalUnits(race != Races::None ? ActionTypes::GetAllActionTypes(race).size() : 0, 0)
    , _goalUnitsMax(race != Races::None ? ActionTypes::GetAllActionTypes(race).size() : 0, 0)
    , _race(race)
{
 
}

void BuildOrderSearchGoal::calculateSupplyRequired()
{
    _supplyRequiredVal = 0;
    for (ActionID a(0); a<_goalUnits.size(); ++a)
    {
        _supplyRequiredVal += _goalUnits[a] * ActionTypes::GetActionType(_race, a).supplyRequired();
    }
}

bool BuildOrderSearchGoal::operator == (const BuildOrderSearchGoal & g)
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

void BuildOrderSearchGoal::setGoal(const ActionType & a, const UnitCountType num)
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnits.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    _goalUnits[a.ID()] = num;

    calculateSupplyRequired();
}

bool BuildOrderSearchGoal::hasGoal() const
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

void BuildOrderSearchGoal::setGoalMax(const ActionType & a, const UnitCountType num)
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnitsMax.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    _goalUnitsMax[a.ID()] = num;
}

UnitCountType BuildOrderSearchGoal::getGoal(const ActionType & a) const
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnits.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    return _goalUnits[a.ID()];
}

UnitCountType BuildOrderSearchGoal::getGoalMax(const ActionType & a) const
{
    BOSS_ASSERT(a.ID() >= 0 && a.ID() < _goalUnitsMax.size(), "Action type not valid");
    BOSS_ASSERT(a.getRace() == _race, "Action type race doesn't match this goal object");

    return _goalUnitsMax[a.ID()];
}

SupplyCountType BuildOrderSearchGoal::supplyRequired() const
{
    return _supplyRequiredVal;
}

void BuildOrderSearchGoal::printGoal() const
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

bool BuildOrderSearchGoal::isAchievedBy(const GameState & state)
{
    static const ActionType & Hatchery      = ActionTypes::GetActionType("Zerg_Hatchery");
    static const ActionType & Lair          = ActionTypes::GetActionType("Zerg_Lair");
    static const ActionType & Hive          = ActionTypes::GetActionType("Zerg_Hive");
    static const ActionType & Spire         = ActionTypes::GetActionType("Zerg_Spire");
    static const ActionType & GreaterSpire  = ActionTypes::GetActionType("Zerg_Greater_Spire");

    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);

        int have = state.getUnitData().getNumTotal(actionType);

        if (state.getRace() == Races::Zerg)
        {
            if (actionType == Hatchery)
            {
                have += state.getUnitData().getNumTotal(Lair);
                have += state.getUnitData().getNumTotal(Hive);
            }
            else if (actionType == Lair)
            {
                have += state.getUnitData().getNumTotal(Hive);
            }
            else if (actionType == Spire)
            {
                have += state.getUnitData().getNumTotal(GreaterSpire);
            }
        }

        if (have < getGoal(actionType))
        {
            return false;
        }
    }

    return true;
}