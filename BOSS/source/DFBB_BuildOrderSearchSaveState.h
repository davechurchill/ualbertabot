#pragma once

#include "Common.h"
#include "ActionType.h"

#define MAX_SAVE_ACTIONS 100

namespace BOSS
{
class DFBB_BuildOrderSearchSaveState
{

	int		depth;
	int		upperBound;
	int		currentActions[MAX_SAVE_ACTIONS];

public:

	DFBB_BuildOrderSearchSaveState();

	DFBB_BuildOrderSearchSaveState(const std::vector<ActionType> & buildOrder, int ub);
	
	int getUpperBound() const;
	int operator [] (const int index) const;
	int getDepth() const;
	int getAction(const int d) const;

	void print() const;
};
}