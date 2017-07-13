#include "UnitImpl.h"
#include <BWAPI\UnitCommand.h>

AKBot::UnitImpl::UnitImpl(int _id, BWAPI::UnitData data)
	: id(_id)
	, data(data)
	, self(&data)
{
	clear();
}
//--------------------------------------------- ISSUE COMMAND ----------------------------------------------
bool AKBot::UnitImpl::issueCommand(BWAPI::UnitCommand command)
{
	if (!canIssueCommand(command))
		return false;

	command.unit = this;

	// If using train or morph on a hatchery, automatically switch selection to larva
	// (assuming canIssueCommand ensures that there is a larva)
	if ((command.type == UnitCommandTypes::Train ||
		command.type == UnitCommandTypes::Morph) &&
		getType().producesLarva() && command.getUnitType().whatBuilds().first == UnitTypes::Zerg_Larva)
	{
		Unitset larvae(this->getLarva());
		for (Unit larva : larvae)
		{
			if (!larva->isConstructing() && larva->isCompleted() && larva->canCommand())
			{
				command.unit = larva;
				break;
			}
		}
		if (command.unit == this)
			return false;
	}

	/*BWAPIC::UnitCommand c;
	c.type = command.type;
#pragma warning(suppress: 6011)
	c.unitIndex = command.unit->getID();
	if (command.target)
		c.targetIndex = command.target->getID();
	else
		c.targetIndex = -1;
	c.x = command.x;
	c.y = command.y;
	c.extra = command.extra;

	Command(command).execute(0);
	static_cast<GameImpl*>(BroodwarPtr)->addUnitCommand(c);
	lastCommandFrame = Broodwar->getFrameCount();*/
	lastCommand = command;
	return true;
}
