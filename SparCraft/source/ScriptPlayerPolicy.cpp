#include "ScriptPlayerPolicy.h"

using namespace SparCraft;

ScriptPlayerPolicy::ScriptPlayerPolicy()
    : _allowOverkill(true)
{

}

ScriptPlayerPolicy::ScriptPlayerPolicy(const ScriptPolicy & oor, const ScriptPolicy & ir, const ScriptPolicy & r, bool allowOverkill)
    : _oorPolicy(oor)
    , _irPolicy(ir)
    , _reloadPolicy(r)
    , _allowOverkill(allowOverkill)
{

}

const ScriptPolicy & ScriptPlayerPolicy::getOutOfRangePolicy() const
{
    return _oorPolicy;
}

const ScriptPolicy & ScriptPlayerPolicy::getInRangePolicy() const
{
    return _irPolicy;
}

const ScriptPolicy & ScriptPlayerPolicy::getReloadPolicy() const
{
    return _reloadPolicy;
}

bool ScriptPlayerPolicy::getAllowOverkill()
{
    return _allowOverkill;
}

ScriptPolicy::ScriptPolicy()
    : _actionType       (0)
    , _angle            (0)
    , _distance         (0)
{

}

int ScriptPolicy::getActionType() const
{
    return _actionType;
}

int ScriptPolicy::getAngle() const
{
    return _angle;
}

int ScriptPolicy::getDistance() const
{
    return _distance;
}

const ScriptPolicyTarget & ScriptPolicy::getTarget() const
{
    return _target;
}

ScriptPolicy::ScriptPolicy(const rapidjson::Value & value)
    : ScriptPolicy()
{
    SPARCRAFT_ASSERT(value.IsArray(), "ScriptPolicy is not Array");
    SPARCRAFT_ASSERT(value[0].IsString(), "ScriptPolicy[0] is not String");
    const std::string & actionString = value[0].GetString();

    if (actionString == "Attack")
    {
        _actionType = PolicyAction::Attack;
        SPARCRAFT_ASSERT(value[1].IsArray(), "ScriptPolicy Attack[1] should be target array");
        _target = ScriptPolicyTarget(value[1]);
    }
    else if (actionString == "Move")
    {
        SPARCRAFT_ASSERT(value.Size() == 4, "Move policy must have 4 elements: [Move, Target, Angle, Distance]");

        _actionType = PolicyAction::Move;
        SPARCRAFT_ASSERT(value[1].IsArray(), "ScriptPolicy Move[1] should be target array");
        _target = ScriptPolicyTarget(value[1]);

        SPARCRAFT_ASSERT(value[2].IsInt(), "ScriptPolicy Move[2] should be int (angle, degrees)");
        _angle = value[2].GetInt();

        SPARCRAFT_ASSERT(value[3].IsInt(), "ScriptPolicy Move[3] should be int (distance, pixels)");
        _distance = value[3].GetInt();
    }
    else if (actionString == "Reload")
    {
        _actionType = PolicyAction::Reload;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown ScriptPolicy Action String: %s", actionString.c_str());
    }
}


ScriptPolicyTarget::ScriptPolicyTarget()
    : targetPlayer          (0)
    , targetType            (0)
    , targetOperator        (0)
{
    
}
 
ScriptPolicyTarget::ScriptPolicyTarget(const rapidjson::Value & value)
    : ScriptPolicyTarget()
{
    SPARCRAFT_ASSERT(value.IsArray(), "ScriptPolicyTarget is not an array");

    SPARCRAFT_ASSERT(value[0].IsString(), "ScriptPolicyTarget[0] is not String");
    targetPlayer = GetTargetPlayer(value[0].GetString());

    SPARCRAFT_ASSERT(value[1].IsString(), "ScriptPolicyTarget[1] is not String");
    targetType = GetTargetType(value[1].GetString());

    if (targetType == PolicyTargetType::Unit)
    {
        SPARCRAFT_ASSERT(value[2].IsString(), "ScriptPolicyTarget[2] is not String");
        targetOperator = GetTargetOperator(value[2].GetString());

        SPARCRAFT_ASSERT(value[3].IsArray(), "ScriptPolicyTarget[3] is not Array");

        for (size_t i(0); i < value[3].Size(); ++i)
        {
            targetOperandSigns.push_back(GetTargetOperandSign(value[3][i].GetString()));
            targetOperands.push_back(GetTargetOperand(value[3][i].GetString()));
        }
    }
}

int ScriptPolicyTarget::GetTargetType(const std::string & string)
{
    if (string == "Unit")
    {
        return PolicyTargetType::Unit;
    }
    else if (string == "Center")
    {
        return PolicyTargetType::Center;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown PolicyTargetType String: %s", string.c_str());
    }

    return PolicyTargetType::Default;
}

int ScriptPolicyTarget::GetTargetPlayer(const std::string & string)
{
    if (string == "Self")
    {
        return PolicyTargetPlayer::Self;
    }
    else if (string == "Enemy")
    {
        return PolicyTargetPlayer::Enemy;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown PolicyTargetPlayer String: %s", string.c_str());
    }

    return PolicyTargetPlayer::Default;
}

int ScriptPolicyTarget::GetTargetOperator(const std::string & string)
{
    if (string == "Min")
    {
        return PolicyOperator::Min;
    }
    else if (string == "Max")
    {
        return PolicyOperator::Max;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown PolicyOperator String: %s", string.c_str());
    }

    return PolicyOperator::Default;
}

int ScriptPolicyTarget::GetTargetOperand(const std::string & string)
{
    if (string.substr(1) == "Distance")
    {
        return PolicyOperand::Distance;
    }
    else if (string.substr(1) == "HP")
    {
        return PolicyOperand::HP;
    }
    else if (string.substr(1) == "DPS")
    {
        return PolicyOperand::DPS;
    }
    else if (string.substr(1) == "Threat")
    {
        return PolicyOperand::Threat;
    }
    else if (string.substr(1) == "Focus")
    {
        return PolicyOperand::Focus;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown PolicyOperand String: %s", string.c_str());
    }

    return PolicyOperand::Default;
}

int ScriptPolicyTarget::GetTargetOperandSign(const std::string & string)
{
    if (string[0] == '+')
    {
        return 1;
    }
    else if (string[0] == '-')
    {
        return -1;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "PolicyOperand Must begin with '+' or '-' to indicate sign: %s", string.c_str());
    }

    return PolicyOperand::Default;
}