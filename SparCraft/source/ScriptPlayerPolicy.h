#pragma once

#include "Common.h"
#include "rapidjson/document.h"
#include <vector>

namespace SparCraft
{

namespace PolicyAction
{
    enum { Default, Attack, Move, Reload, Pass };
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
    enum { Default, Distance, DPS, Threat, HP, Focus };
}

class ScriptPolicyTarget
{
public:
    int     targetPlayer;
    int     targetType;
    int     targetOperator;
    std::vector<int> targetOperands;
    std::vector<int> targetOperandSigns;

    ScriptPolicyTarget();
    ScriptPolicyTarget(const rapidjson::Value & value);

    static int GetTargetType        (const std::string & string);
    static int GetTargetPlayer      (const std::string & string);
    static int GetTargetOperator    (const std::string & string);
    static int GetTargetOperand     (const std::string & string);
    static int GetTargetOperandSign (const std::string & string);
};

class ScriptPolicy
{
    int                 _actionType;
    int                 _angle;
    int                 _distance;
    int                 _passFrames;
    ScriptPolicyTarget  _target;

public:

    ScriptPolicy();
    ScriptPolicy(const rapidjson::Value & value);

    int getActionType() const;
    int getPassFrames() const;
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
    ScriptPlayerPolicy(const ScriptPolicy & oor, const ScriptPolicy & ir, const ScriptPolicy & r, bool allowOverkill);

    const ScriptPolicy & getOutOfRangePolicy() const;
    const ScriptPolicy & getInRangePolicy() const;    
    const ScriptPolicy & getReloadPolicy() const;
    bool getAllowOverkill();
};


}