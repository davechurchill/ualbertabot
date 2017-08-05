#include "SquadData.h"
#include "Global.h"

using namespace UAlbertaBot;
using namespace AKBot;

SquadData::SquadData(
	AKBot::PlayerLocationProvider& locationProvider,
	shared_ptr<AKBot::OpponentView> opponentView,
	const UnitInfoManager& unitInfo,
	shared_ptr<BaseLocationManager> bases,
	shared_ptr<MapTools> mapTools,
	std::shared_ptr<AKBot::Logger> logger)
	: _locationProvider(locationProvider)
	, _opponentView(opponentView)
	, _unitInfo(unitInfo)
	, _bases(bases)
	, _mapTools(mapTools)
	, _logger(logger)
{
	
}

void SquadData::update(shared_ptr<MapTools> map, int currentFrame)
{
	updateAllSquads(map, currentFrame);
    verifySquadUniqueMembership();
}

void SquadData::clearSquadData(int currentFrame)
{
	// give back workers who were in squads
    for (auto & kv : _squads)
	{
        Squad & squad = kv.second;
		squad.clear(currentFrame);
	}

	_squads.clear();
}

void SquadData::removeSquad(const std::string & squadName, int currentFrame)
{
    auto squadPtr = _squads.find(squadName);

    UAB_ASSERT_WARNING(squadPtr != _squads.end(), "Trying to clear a squad that didn't exist: %s", squadName.c_str());
    if (squadPtr == _squads.end())
    {
        return;
    }

	auto& squad = squadPtr->second;
	squad.clear(currentFrame);

    _squads.erase(squadName);
}

const std::map<std::string, Squad> & SquadData::getSquads() const
{
    return _squads;
}

void UAlbertaBot::SquadData::onUnitRemoved(UnitHandler handler)
{
	onRemoveHandler = handler;
}

bool SquadData::squadExists(const std::string & squadName)
{
    return _squads.find(squadName) != _squads.end();
}

void SquadData::addSquad(const std::string & squadName, Squad & squad)
{
	squad.onUnitRemoved(onRemoveHandler);
	_squads.insert(std::make_pair(squadName, squad));
}

void SquadData::addSquad(const std::string & squadName, const SquadOrder & squadOrder, size_t priority)
{
	Squad squad(squadName, squadOrder, priority, _locationProvider, _opponentView, _unitInfo, _bases, _mapTools, _logger);
	addSquad(squadName, squad);
}

void SquadData::updateAllSquads(shared_ptr<MapTools> map, int currentFrame)
{
	for (auto & kv : _squads)
	{
		auto& squad = kv.second;
		squad.update(map, currentFrame);
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
                _logger->log("Unit is in at least two squads: %s", unit->getType().getName().c_str());
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
	UAB_ASSERT_WARNING(squadExists(squadName), "Trying to access squad that doesn't exist: %s", squadName.c_str());
    if (!squadExists(squadName))
    {
        int a = 10;
    }

    return _squads.find(squadName)->second;
}