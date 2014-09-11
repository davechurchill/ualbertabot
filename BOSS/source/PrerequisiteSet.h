#pragma once

#include "Common.h"
#include "Constants.h"
#include "Array.hpp"
#include "ActionType.h"

namespace BOSS
{

class ActionCountPair
{
    ActionType      _action;
    UnitCountType   _count;

public:

    ActionCountPair();

    ActionCountPair(const ActionType & action, const UnitCountType count);

    const ActionType & getAction() const;

    const UnitCountType & getCount() const;
};

class PrerequisiteSet
{
	Vec<ActionCountPair, Constants::MAX_ACTION_TYPES> _actionCounts;

public:

	PrerequisiteSet();

    const size_t size() const;
    const bool isEmpty() const;
    const bool contains(const ActionType & action) const;
    const ActionType & getActionType(const UnitCountType index) const;
    const UnitCountType & getActionTypeCount(const UnitCountType index) const;
    
    void add(const ActionType & action, const UnitCountType count = 1);
    void addUnique(const ActionType & action, const UnitCountType count = 1);
    void addUnique(const PrerequisiteSet & set);
    void remove(const ActionType & action);
    void remove(const PrerequisiteSet & set);

    const std::string toString() const;
};

}