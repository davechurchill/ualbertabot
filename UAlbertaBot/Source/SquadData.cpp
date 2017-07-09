#include "SquadData.h"
#include "Global.h"

using namespace UAlbertaBot;

SquadData::SquadData() 
{
	
}

void SquadData::update(const MapTools& map)
{
	updateAllSquads(map);
    verifySquadUniqueMembership();
}

void SquadData::clearSquadData()
{
    // give back workers who were in squads
    for (auto & kv : _squads)
	{
        Squad & squad = kv.second;

        auto & units = squad.getUnits();

        for (auto & unit : units)
        {
            if (unit->getType().isWorker())
            {
                Global::Workers().finishedWithWorker(unit);
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

    for (auto & unit : squadPtr->second.getUnits())
    {
        if (unit->getType().isWorker())
        {
            Global::Workers().finishedWithWorker(unit);
        }
    }

    _squads.erase(squadName);
}

const std::map<std::string, Squad> & SquadData::getSquads() const
{
    return _squads;
}

bool SquadData::squadExists(const std::string & squadName)
{
    return _squads.find(squadName) != _squads.end();
}

void SquadData::addSquad(const std::string & squadName, const Squad & squad)
{
	_squads[squadName] = squad;
}

void SquadData::addSquad(const std::string & squadName, const SquadOrder & squadOrder, size_t priority)
{
	_squads[squadName] = Squad(squadName, squadOrder, priority);
}

void SquadData::updateAllSquads(const MapTools& map)
{
	for (auto & kv : _squads)
	{
		kv.second.update(map);
	}
}

void SquadData::drawSquadInformation(AKBot::ScreenCanvas& canvas, int x, int y)
{
    if (!Config::Debug::DrawSquadInfo)
    {
        return;
    }

	canvas.drawTextScreen(x, y, "\x04Squads");
	canvas.drawTextScreen(x, y+20, "\x04NAME");
	canvas.drawTextScreen(x+150, y+20, "\x04SIZE");
	canvas.drawTextScreen(x+200, y+20, "\x04LOCATION");

	int yspace = 0;

	for (auto & kv : _squads) 
	{
        const Squad & squad = kv.second;

		auto & units = squad.getUnits();
		const SquadOrder & order = squad.getSquadOrder();

		canvas.drawTextScreen(x, y+40+((yspace)*10), "\x03%s", squad.getName().c_str());
		canvas.drawTextScreen(x+150, y+40+((yspace)*10), "\x03%d", units.size());
		canvas.drawTextScreen(x+200, y+40+((yspace++)*10), "\x03(%d,%d)", order.getPosition().x, order.getPosition().y);

		canvas.drawCircleMap(order.getPosition(), 10, BWAPI::Colors::Green, true);
        canvas.drawCircleMap(order.getPosition(), order.getRadius(), BWAPI::Colors::Red, false);
        canvas.drawTextMap(order.getPosition() + BWAPI::Position(0, 12), "%s", squad.getName().c_str());

        for (const BWAPI::Unit unit : units)
        {
            canvas.drawTextMap(unit->getPosition() + BWAPI::Position(0, 10), "%s", squad.getName().c_str());
        }
	}
}

void SquadData::verifySquadUniqueMembership()
{
    std::vector<BWAPI::Unit> assigned;

    for (const auto & kv : _squads)
    {
        //std::cout << "Squad: "kv.second.getName()
        for (auto & unit : kv.second.getUnits())
        {
            if (std::find(assigned.begin(), assigned.end(), unit) != assigned.end())
            {
                BWAPI::Broodwar->printf("Unit is in at least two squads: %s", unit->getType().getName().c_str());
            }

            assigned.push_back(unit);
        }
    }
}

bool SquadData::unitIsInSquad(BWAPI::Unit unit) const
{
    return getUnitSquad(unit) != nullptr;
}

const Squad * SquadData::getUnitSquad(BWAPI::Unit unit) const
{
    for (const auto & kv : _squads)
    {
        if (kv.second.containsUnit(unit))
        {
            return &kv.second;
        }
    }

    return nullptr;
}

Squad * SquadData::getUnitSquad(BWAPI::Unit unit)
{
    for (auto & kv : _squads)
    {
        if (kv.second.containsUnit(unit))
        {
            return &kv.second;
        }
    }

    return nullptr;
}

void SquadData::assignUnitToSquad(BWAPI::Unit unit, Squad & squad)
{
    UAB_ASSERT_WARNING(canAssignUnitToSquad(unit, squad), "We shouldn't be re-assigning this unit!");

    Squad * previousSquad = getUnitSquad(unit);

    if (previousSquad)
    {
        previousSquad->removeUnit(unit);
    }

    squad.addUnit(unit);
}

bool SquadData::canAssignUnitToSquad(BWAPI::Unit unit, const Squad & squad) const
{
    const Squad * unitSquad = getUnitSquad(unit);

    // make sure strictly less than so we don't reassign to the same squad etc
    return !unitSquad || (unitSquad->getPriority() < squad.getPriority());
}

Squad & SquadData::getSquad(const std::string & squadName)
{
    UAB_ASSERT_WARNING(squadExists(squadName), "Trying to access squad that doesn't exist: %s", squadName);
    if (!squadExists(squadName))
    {
        int a = 10;
    }

    return _squads[squadName];
}