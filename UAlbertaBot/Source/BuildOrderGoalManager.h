#pragma once

#include "Common.h"
#include "MetaType.h"

namespace UAlbertaBot
{
class BuildOrderGoalItem
{
	// the MetaType to be constructed
	MetaType	_metaType;

	// the number we wish to construct
	int			_num;

public:

	// constructor
	BuildOrderGoalItem(const MetaType & t, const int n) : _metaType(t), _num(n) {}

	// getters
	const MetaType & metaType()	const	{ return _metaType; }
	const int num() const				{ return _num; }


};

class BuildOrderGoal
{
	// the goals
	std::vector<BuildOrderGoalItem>		goal;

	// priority
	int priority;

public:
	
	BuildOrderGoal() : priority(0) {}
	BuildOrderGoal(const int p) : priority(p) {}
	~BuildOrderGoal() {}

	void addItem(const BuildOrderGoalItem & bogi)
	{
		goal.push_back(bogi);
	}

	void setPriority(const int p)
	{
		priority = p;
	}

	const int getPriority() const
	{
		return priority;
	}

	const std::vector<BuildOrderGoalItem> & getGoal() const
	{
		return goal;
	}

	bool operator < (const BuildOrderGoal & bog)
	{
		return getPriority() < bog.getPriority();
	}
};

class BuildOrderGoalManager {

	std::vector<BuildOrderGoal>		goals;

	// add a build order goal item with a priority
	void				addGoal(const MetaType t, int num, int p);

	// checks to see if a goal is completed by using BWAPI data
	bool				isCompleted(const BuildOrderGoal & bog) const;
	
	// set the build order goals based on expert knowledge
	void				setBuildOrderGoals();

public:

	BuildOrderGoalManager();
	~BuildOrderGoalManager();

	// gets the highest priority goal which isn't completed
	BuildOrderGoal &	getNextBuildOrderGoal() const;
};
}