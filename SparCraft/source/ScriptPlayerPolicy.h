#pragma once

#include "Common.h"
#include "rapidjson\document.h"

namespace SparCraft
{

namespace PolicyAction
{
    enum { Default, Attack, Move, None };
}

namespace PolicyTargetPlayer
{
    enum { Default, Self, Enemy };
}

namespace PolicyTargetType
{
    enum { Default, Unit, Center };
}

namespace PolicyOperator
{
    enum { Default, Min, Max };
}

namespace PolicyOperand
{
    enum { Default, Distance, DPS, Threat, HP };
}


class ScriptPolicyTarget
{
public:
    int     targetPlayer;
    int     targetType;
    int     targetOperator;
    int     targetOperand;
    int     targetOperandPlayer;
    int     targetOperandType;

    ScriptPolicyTarget();
    ScriptPolicyTarget(const rapidjson::Value & value);

    static int GetTargetType(const std::string & string);
    static int GetTargetPlayer(const std::string & string);
    static int GetTargetOperator(const std::string & string);
    static int GetTargetOperand(const std::string & string);
};

class ScriptPolicy
{
    int                 _actionType;
    int                 _angle;
    int                 _distance;
    ScriptPolicyTarget  _target;

public:

    ScriptPolicy();
    ScriptPolicy(const rapidjson::Value & value);

    int getActionType() const;
    int getAngle() const;
    int getDistance() const;
    const ScriptPolicyTarget & getTarget() const;

    static int GetTargetType(const std::string & targetString);
};


class ScriptPlayerPolicy 
{
    ScriptPolicy    _oorPolicy;
    ScriptPolicy    _irPolicy;
    ScriptPolicy    _reloadPolicy;
    bool            _allowOverkill;

public:
    
    ScriptPlayerPolicy();
    ScriptPlayerPolicy(const rapidjson::Value & value);

    const ScriptPolicy & getOutOfRangePolicy() const;
    const ScriptPolicy & getInRangePolicy() const;    
    const ScriptPolicy & getReloadPolicy() const;
    bool getAllowOverkill();
};


}