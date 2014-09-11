#pragma once

#include "Common.h"
#include "Constants.h"
#include "Array.hpp"
#include "ActionType.h"

namespace BOSS
{

class ActionSet
{
	Vec<ActionType, Constants::MAX_ACTION_TYPES> _actionTypes;

public:

	ActionSet();

    const size_t size() const;
    const bool isEmpty() const;
    const bool contains(const ActionType & type) const;

    const ActionType & operator [] (const size_t & index) const;
    ActionType & operator [] (const size_t & index);

    void add(const ActionType & action);
    void addAllActions(const RaceID & race);
    void remove(const ActionType & action);
    void clear();
};

}