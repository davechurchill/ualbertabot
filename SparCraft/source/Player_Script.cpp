#include "Player_Script.h"
#include "Timer.h"

const double PI = 3.141592653589793238463;
const double RAD = 0.01745329251;

using namespace SparCraft;

Player_Script::Player_Script (const size_t & playerID, const ScriptPlayerPolicy & playerPolicy)
    : Player()
{
	_playerID = playerID;
    _enemyID = (playerID + 1) % 2;
    _playerCentersCalculated[0] = false;
    _playerCentersCalculated[1] = false;
    _playerPolicy = playerPolicy;
}

void Player_Script::getMove(const GameState & state, Move & move)
{
    startTimer();

    move.clear();
    _playerCentersCalculated[0] = false;
    _playerCentersCalculated[1] = false;
    
    std::vector<size_t> allEnemyUnits = state.getUnitIDs(_enemyID);
    
    _numTargeting.assign(state.getAllUnits().size(), 0);
    _damageAssigned.assign(state.getAllUnits().size(), 0);

    for (size_t u(0); u < state.numUnits(_playerID); ++u)
    {
        const Unit & unit = state.getUnit(_playerID, u);

        // If the unit can't act yet, then go to the next one
        if (unit.firstTimeFree() > state.getTime())
        {
            continue;
        }

        getEnemyUnitsInAttackRange(unit, state, allEnemyUnits, _enemyUnitsInAttackRange);

        Action unitAction;

        // If we can't currently attack any enemy units
        if (_enemyUnitsInAttackRange.empty())
        {
            SPARCRAFT_ASSERT(_playerPolicy.getOutOfRangePolicy().getActionType() != PolicyAction::Attack, "We can only Move if out of range");

            unitAction = getPolicyAction(state, unit, _playerPolicy.getOutOfRangePolicy(), allEnemyUnits);
        }
        // We are able to attack at least one enemy unit
        else
        {
            // If this is not the Reload phase, implement the InRange policy
            if (unit.nextAttackActionTime() == unit.nextMoveActionTime())
            {
                unitAction = getPolicyAction(state, unit, _playerPolicy.getInRangePolicy(), _enemyUnitsInAttackRange);
            }
            // Otherwise this is Reload phase, so implement the Reload Policy
            else
            {
                SPARCRAFT_ASSERT(_playerPolicy.getOutOfRangePolicy().getActionType() != PolicyAction::Attack, "We can't attack if we're currently reloading");

                unitAction = getPolicyAction(state, unit, _playerPolicy.getReloadPolicy(), _enemyUnitsInAttackRange);
            }
        }

        move.addAction(unitAction);

        if (unitAction.type() == ActionTypes::ATTACK)
        {
            const Unit & attacker = state.getUnitByID(unitAction.getID());
            const Unit & target = state.getUnitByID(unitAction.getTargetID());

            _numTargeting[target.getID()]++;
            _damageAssigned[target.getID()] += target.damageTakenFrom(attacker);

            // overkill is not allowed and it has been reached
            if (!_playerPolicy.getAllowOverkill() && allEnemyUnits.size() > 1 &&  (int)_damageAssigned[target.getID()] >= target.currentHP())
            {
                allEnemyUnits.erase(std::remove(allEnemyUnits.begin(), allEnemyUnits.end(), target.getID()), allEnemyUnits.end());
            }
        }

    }

    stopTimer();
}

Action Player_Script::getPolicyAction(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets)
{
    if (policy.getActionType() == PolicyAction::Move)
    {
        const Position targetPos = getPolicyTargetPosition(state, myUnit, policy, validUnitTargets);
        const Position & unitPos = myUnit.currentPosition(state.getTime());
        double angleRad = policy.getAngle() * RAD;

        double distance = policy.getDistance();
        double dX = targetPos.x() - unitPos.x();
        double dY = targetPos.y() - unitPos.y();
        double length = sqrt(dX*dX + dY*dY);

        if (length == 0)
        {
            return Action(myUnit.getID(), _playerID, ActionTypes::MOVE, 0, unitPos);
        }

        dX = dX / length * distance;
        dY = dY / length * distance;
        
        double pX = dX * cos(angleRad) - dY * sin(angleRad);
        double pY = dX * sin(angleRad) + dY * cos(angleRad);

        Position movePos(unitPos.x() + pX, unitPos.y() + pY);

        return Action(myUnit.getID(), _playerID, ActionTypes::MOVE, 0, movePos);
    }
    else if (policy.getActionType() == PolicyAction::Attack)
    {
        return Action(myUnit.getID(), _playerID, ActionTypes::ATTACK, getPolicyTargetUnitID(state, myUnit, policy, validUnitTargets));
    }
    else if (policy.getActionType() == PolicyAction::Reload)
    {
        return Action(myUnit.getID(), _playerID, ActionTypes::RELOAD, 0);
    }
    else if (policy.getActionType() == PolicyAction::Pass)
    {
        return Action(myUnit.getID(), _playerID, ActionTypes::PASS, policy.getPassFrames());
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown Policy Action Type: %d", policy.getActionType());
        return Action();
    }
}

Position Player_Script::getPolicyTargetPosition(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets)
{
    SPARCRAFT_ASSERT(policy.getActionType() == PolicyAction::Move, "We can only get a target Position if it's a Move action");
 
    Position pos;
    const ScriptPolicyTarget & target = policy.getTarget();

    if (target.targetType == PolicyTargetType::Unit)
    {
        pos = state.getUnitByID(getPolicyTargetUnitID(state, myUnit, policy, validUnitTargets)).currentPosition(state.getTime());
    }
    else if (target.targetType == PolicyTargetType::Center)
    {
        const size_t playerCenter = (target.targetPlayer == PolicyTargetPlayer::Self) ? _playerID : _enemyID;
        pos = getPlayerCenter(state, playerCenter);
    }

    return pos;
}

size_t Player_Script::getPolicyTargetUnitID(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets)
{
    const ScriptPolicyTarget & target = policy.getTarget();

    SPARCRAFT_ASSERT(target.targetType == PolicyTargetType::Unit, "Can't get a Unit ID if the target policy doesn't specify a unit");
    SPARCRAFT_ASSERT(validUnitTargets.size() > 0, "Can't get a Unit ID if the candidate target list is empty");

    bool min = (target.targetOperator == PolicyOperator::Min);

    _bestVals.assign(policy.getTarget().targetOperands.size(), min ? std::numeric_limits<double>::max() : std::numeric_limits<double>::lowest());
    size_t bestUnitID = 0;

    for (const size_t & unitID : validUnitTargets)
    {
        const Unit & targetUnit = state.getUnitByID(unitID);

        getTargetOperandValue(state, myUnit, policy, targetUnit, _val);

        if ((min && less(_val, _bestVals)) || (!min && greater(_val, _bestVals)))
        {
            _bestVals.assign(_val.begin(), _val.end());
            bestUnitID = targetUnit.getID();
        }
    }

    return bestUnitID;
}

void Player_Script::getTargetOperandValue(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const Unit & targetUnit, std::vector<double> & val)
{
    const std::vector<int> & operands = policy.getTarget().targetOperands;
    const std::vector<int> & signs = policy.getTarget().targetOperandSigns;
    val.clear();

    for (size_t i(0); i < operands.size(); ++i)
    {
        switch (operands[i])
        {
            case PolicyOperand::Distance: { val.push_back(signs[i] * myUnit.getDistanceSqToUnit(targetUnit, state.getTime())); break; }
            case PolicyOperand::HP:       { val.push_back(signs[i] * targetUnit.currentHP()); break; }
            case PolicyOperand::DPS:      { val.push_back(signs[i] * myUnit.dpf()); break; }
            case PolicyOperand::Threat:   { val.push_back(signs[i] * myUnit.dpf() / myUnit.currentHP()); break; }
            case PolicyOperand::Focus:    { val.push_back(signs[i] * _numTargeting[targetUnit.getID()]); break; }
            default:                      { SPARCRAFT_ASSERT(false, "Unknown policy operand: %d", operands[i]); }
        }
    }
}

bool Player_Script::greater(const std::vector<double> & v1, const std::vector<double> & v2)
{
    for (size_t i(0); i < v1.size(); ++i)
    {
        if (v1[i] > v2[i])
        {
            return true;
        } 
        else if (v1[i] < v2[i])
        {
            return false;
        }
    }

    return false;
}

bool Player_Script::less(const std::vector<double> & v1, const std::vector<double> & v2)
{
    for (size_t i(0); i < v1.size(); ++i)
    {
        if (v1[i] < v2[i])
        {
            return true;
        }
        else if (v1[i] > v2[i])
        {
            return false;
        }
    }

    return false;
}

const Position & Player_Script::getPlayerCenter(const GameState & state, const size_t & playerID)
{
    if (!_playerCentersCalculated[playerID])
    {
        _playerCenters[playerID] = AITools::CalculatePlayerCenter(state, playerID);
        _playerCentersCalculated[playerID] = true;
    }

    return _playerCenters[playerID];
}

void Player_Script::getEnemyUnitsInAttackRange(const Unit & myUnit, const GameState & state, const std::vector<size_t> & enemyUnits, std::vector<size_t> & unitIDs)
{
    const size_t enemy = state.getEnemy(_playerID);
    unitIDs.clear();
    
    for (const size_t & enemyUnitID : enemyUnits)
    {
        const Unit & enemyUnit = state.getUnitByID(enemyUnitID);
        
        if (myUnit.canAttackTarget(enemyUnit, state.getTime()))
        {
            unitIDs.push_back(enemyUnitID);
        }
    }
}

PlayerPtr Player_Script::clone()
{
    return PlayerPtr(new Player_Script(*this));
}

const std::string & Player_Script::getDescription()
{
    if (_description.size() == 0)
    {
        _description = _name;
    }

    return _description;
}