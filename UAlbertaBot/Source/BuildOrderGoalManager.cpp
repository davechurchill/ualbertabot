#include "Common.h"
#include "BuildOrderGoalManager.h"

using namespace UAlbertaBot;

BuildOrderGoalManager::BuildOrderGoalManager()
{
	
}

bool BuildOrderGoalManager::isCompleted(const BuildOrderGoal & bog) const
{
	// for each item in the goal
	for (const BuildOrderGoalItem & bogi : bog.getGoal())
	{
		if (bogi.metaType().type() == MetaTypes::Unit)
		{
			// if we do not have that many of the unit type, return false
			if (BWAPI::Broodwar->self()->allUnitCount(bogi.metaType().getUnitType()) < bogi.num())
			{
				return false;
			}
		}
		// if we have not researched that tech, return false
		else if (bogi.metaType().type() == MetaTypes::Tech)
		{
			if (!BWAPI::Broodwar->self()->hasResearched(bogi.metaType().getTechType()))
			{
				return false;
			}
		}
		// if we have not upgraded to that level, return false
		else if (bogi.metaType().type() == MetaTypes::Upgrade)
		{
			if (BWAPI::Broodwar->self()->getUpgradeLevel(bogi.metaType().getUpgradeType()) < bogi.num())
			{
				return false;
			}
		}
	}

	// return true only if everything meets the criteria
	return true;
}

void BuildOrderGoalManager::addGoal(const MetaType t, int num, int p)
{
	// create the build order item
	BuildOrderGoalItem bogi(t, num);

	// see if an item with this priority already exists
	int existingIndex = -1;
	for (int i(0); i<(int)goals.size(); ++i)
	{
		if (goals[i].getPriority() == p)
		{
			existingIndex = i;
			break;
		}
	}

	// if it already exists, add it to that goal
	if (existingIndex != -1)
	{
		goals[existingIndex].addItem(bogi);
	}
	// otherwise create a new goal
	else
	{
		BuildOrderGoal temp(p);
		temp.addItem(bogi);
		goals.push_back(temp);
	}

}

void BuildOrderGoalManager::setBuildOrderGoals()
{
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Nexus), 1, 1000);	// 1	Nexus
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Probe), 1, 1000);	// 1	Probe

	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Zealot),		12,		98);	// 12	Zealot
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Nexus),		2,		96);	// 2	Nexus
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Gateway),	3,		94);	// 3	Gateway
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Zealot),		18,		92);	// 18	Zealot
	addGoal(MetaType(BWAPI::UnitTypes::Protoss_Gateway),	5,		90);

	// sort the goals to make things quicker for us
	std::sort(goals.begin(), goals.end());
}
/*
BuildOrderGoal & BuildOrderGoalManager::getNextBuildOrderGoal() const
{
	
}*/