#include "SquadData.h"

using namespace UAlbertaBot;

SquadData::SquadData() 
{
	
}

void SquadData::update()
{
	updateAllSquads();
    verifySquadUniqueMembership();
}

void SquadData::clearSquadData()
{
    // give back workers who were in squads
    for (auto & kv : _squads)
	{
        Squad & squad = kv.second;

        const BWAPI::Unitset & units = squad.getUnits();

        for (BWAPI::UnitInterface * unit : units)
        {
            if (unit->getType().isWorker())
            {
                WorkerManager::Instance().finishedWithWorker(unit);
            }
        }
	}

	_squads.clear();
}

void SquadData::removeSquad(const std::string & squadName)
{
    auto & squadPtr = _squads.find(squadName);

    UAB_ASSERT_WARNING(squadPtr != _squads.end(), "Trying to clear a squad that didn't exist: %s", squadName.c_str());
    if (squadPtr == _squads.end())
    {
        return;
    }

    for (BWAPI::UnitInterface * unit : squadPtr->second.getUnits())
    {
        if (unit->getType().isWorker())
        {
            WorkerManager::Instance().finishedWithWorker(unit);
        }
    }

    _squads.erase(squadName);
}

bool SquadData::squadExists(const std::string & squadName)
{
    return _squads.find(squadName) != _squads.end();
}

void SquadData::addSquad(const std::string & squadName, const Squad & squad)
{
	_squads[squadName] = squad;
}

void SquadData::updateAllSquads()
{
	for (auto & kv : _squads)
	{
		kv.second.update();
	}
}

void SquadData::drawSquadInformation(int x, int y) 
{
    if (!Config::Debug::DrawSquadInfo)
    {
        return;
    }

	BWAPI::Broodwar->drawTextScreen(x, y, "\x04Squads");
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04NAME");
	BWAPI::Broodwar->drawTextScreen(x+100, y+20, "\x04SIZE");
	BWAPI::Broodwar->drawTextScreen(x+150, y+20, "\x04LOCATION");

	int yspace = 0;

	for (auto & kv : _squads) 
	{
        const Squad & squad = kv.second;

		const BWAPI::Unitset & units = squad.getUnits();
		const SquadOrder & order = squad.getSquadOrder();

		BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03%s", squad.getName().c_str());
		BWAPI::Broodwar->drawTextScreen(x+100, y+40+((yspace)*10), "\x03%d", units.size());
		BWAPI::Broodwar->drawTextScreen(x+150, y+40+((yspace++)*10), "\x03(%d,%d)", order.getPosition().x, order.getPosition().y);

		BWAPI::Broodwar->drawCircleMap(order.getPosition(), 10, BWAPI::Colors::Green, true);
        BWAPI::Broodwar->drawTextMap(order.getPosition() + BWAPI::Position(0, 12), "%s", squad.getName().c_str());
	}
}

void SquadData::verifySquadUniqueMembership()
{
    BWAPI::Unitset assigned;

    for (const auto & kv : _squads)
    {
        for (BWAPI::UnitInterface * unit : kv.second.getUnits())
        {
            if (assigned.contains(unit))
            {
                BWAPI::Broodwar->printf("Unit is in at least two squads: %s", unit->getType().getName().c_str());
            }

            assigned.insert(unit);
        }
    }
}

bool SquadData::unitIsInSquad(BWAPI::UnitInterface * unit) const
{
    return getUnitSquad(unit) != NULL;
}

const Squad * SquadData::getUnitSquad(BWAPI::UnitInterface * unit) const
{
    for (const auto & kv : _squads)
    {
        if (kv.second.getUnits().contains(unit))
        {
            return &kv.second;
        }
    }

    return NULL;
}

Squad * SquadData::getUnitSquad(BWAPI::UnitInterface * unit)
{
    for (auto & kv : _squads)
    {
        if (kv.second.getUnits().contains(unit))
        {
            return &kv.second;
        }
    }

    return NULL;
}

void SquadData::assignUnitToSquad(BWAPI::UnitInterface * unit, Squad & squad)
{
    UAB_ASSERT_WARNING(canAssignUnitToSquad(unit, squad), "We shouldn't be re-assigning this unit!");

    Squad * previousSquad = getUnitSquad(unit);

    if (previousSquad)
    {
        previousSquad->removeUnit(unit);
    }

    squad.addUnit(unit);
}

bool SquadData::canAssignUnitToSquad(BWAPI::UnitInterface * unit, const Squad & squad) const
{
    const Squad * unitSquad = getUnitSquad(unit);

    // make sure strictly less than so we don't reassign to the same squad etc
    return !unitSquad || (unitSquad->getPriority() < squad.getPriority());
}

Squad & SquadData::getSquad(const std::string & squadName)
{
    return _squads[squadName];
}