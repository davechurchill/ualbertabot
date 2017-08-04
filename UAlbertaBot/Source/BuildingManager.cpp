#include "Common.h"
#include "BuildingManager.h"
#include "Micro.h"
#include "Global.h"

using namespace UAlbertaBot;

BuildingManager::BuildingManager(
	const AKBot::OpponentView& opponentView,
	const BaseLocationManager& bases,
	WorkerManager& workerManager,
	const MapTools& mapTools,
	const AKBot::Logger& logger)
    : _debugMode(false)
    , _reservedMinerals(0)
    , _reservedGas(0)
	, _bases(bases)
	, _workerManager(workerManager)
	, _mapTools(mapTools)
	, _opponentView(opponentView)
	, _buildingPlacer(mapTools.getWidth(), mapTools.getHeight(), opponentView, bases, mapTools)
	, _logger(logger)
{

}

// gets called every frame from GameCommander
void BuildingManager::update(int currentFrame)
{
    validateWorkersAndBuildings();				// check to see if assigned workers have died en route or while constructing
    assignWorkersToUnassignedBuildings();		// assign workers to the unassigned buildings and label them 'planned'    
    constructAssignedBuildings(currentFrame);   // for each planned building, if the worker isn't constructing, send the command    
    checkForStartedConstruction(currentFrame);	// check to see if any buildings have started construction and update data structures    
    checkForDeadTerranBuilders();				// if we are terran and a building is under construction without a worker, assign a new one    
    checkForCompletedBuildings(currentFrame);   // check to see if any buildings have completed and update data structures
}

bool BuildingManager::isBeingBuilt(BWAPI::UnitType type)
{
    for (auto & b : _buildings)
    {
        if (b.type == type)
        {
            return true;
        }
    }

    return false;
}

// STEP 1: DO BOOK KEEPING ON WORKERS WHICH MAY HAVE DIED
void BuildingManager::validateWorkersAndBuildings()
{
    // TODO: if a terran worker dies while constructing and its building
    //       is under construction, place unit back into buildingsNeedingBuilders

    std::vector<Building> toRemove;
    
    // find any buildings which have become obsolete
    for (auto & b : _buildings)
    {
        if (b.status != BuildingStatus::UnderConstruction)
        {
            continue;
        }

        if (b.buildingUnit == nullptr || !b.buildingUnit->getType().isBuilding() || b.buildingUnit->getHitPoints() <= 0)
        {
            toRemove.push_back(b);
        }
    }

    removeBuildings(toRemove);
}

// STEP 2: ASSIGN WORKERS TO BUILDINGS WITHOUT THEM
void BuildingManager::assignWorkersToUnassignedBuildings()
{
    // for each building that doesn't have a builder, assign one
    for (Building & b : _buildings)
    {
        if (b.status != BuildingStatus::Unassigned)
        {
            continue;
        }

        if (_debugMode)
		{ 
			_logger.log("Assigning Worker To: %s",b.type.getName().c_str()); 
		}

        // grab a worker unit from WorkerManager which is closest to this final position
        BWAPI::TilePosition testLocation = getBuildingLocation(b);
        if (!testLocation.isValid())
        {
            continue;
        }

        b.finalPosition = testLocation;

        // grab the worker unit from WorkerManager which is closest to this final position
		auto builderUnit = _workerManager.getBuilder(b);
		if (builderUnit) {
			_workerManager.setBuildingWorker(builderUnit, b);
		}

		b.builderUnit = builderUnit;
        if (!b.builderUnit)
        {
            continue;
        }

        // reserve this building's space
        _buildingPlacer.reserveTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());

        b.status = BuildingStatus::Assigned;
    }
}

// STEP 3: ISSUE CONSTRUCTION ORDERS TO ASSIGN BUILDINGS AS NEEDED
void BuildingManager::constructAssignedBuildings(int currentFrame)
{
    for (auto & b : _buildings)
    {
        if (b.status != BuildingStatus::Assigned)
        {
            continue;
        }

        // if that worker is not currently constructing
        if (!b.builderUnit->isConstructing())
        {
            // if we haven't explored the build position, go there
            if (!isBuildingPositionExplored(b))
            {
                Micro::SmartMove(b.builderUnit,BWAPI::Position(b.finalPosition), currentFrame);
            }
            // if this is not the first time we've sent this guy to build this
            // it must be the case that something was in the way of building
            else if (b.buildCommandGiven)
            {
                // tell worker manager the unit we had is not needed now, since we might not be able
                // to get a valid location soon enough
				_workerManager.finishedWithWorker(b.builderUnit, currentFrame);

                // free the previous location in reserved
                _buildingPlacer.freeTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());

                // nullify its current builder unit
                b.builderUnit = nullptr;

                // reset the build command given flag
                b.buildCommandGiven = false;

                // add the building back to be assigned
                b.status = BuildingStatus::Unassigned;
            }
            else
            {
                // issue the build order!
                b.builderUnit->build(b.type,b.finalPosition);

                // set the flag to true
                b.buildCommandGiven = true;
            }
        }
    }
}

// STEP 4: UPDATE DATA STRUCTURES FOR BUILDINGS STARTING CONSTRUCTION
void BuildingManager::checkForStartedConstruction(int currentFrame)
{
    // for each building unit which is being constructed
    for (auto & buildingStarted : _opponentView.self()->getUnits())
    {
        // filter out units which aren't buildings under construction
        if (!buildingStarted->getType().isBuilding() || !buildingStarted->isBeingConstructed())
        {
            continue;
        }

        // check all our building status objects to see if we have a match and if we do, update it
        for (auto & b : _buildings)
        {
            if (b.status != BuildingStatus::Assigned)
            {
                continue;
            }
        
            // check if the positions match
            if (b.finalPosition == buildingStarted->getTilePosition())
            {
                // the resources should now be spent, so unreserve them
                _reservedMinerals -= buildingStarted->getType().mineralPrice();
                _reservedGas      -= buildingStarted->getType().gasPrice();

                // flag it as started and set the buildingUnit
                b.underConstruction = true;
                b.buildingUnit = buildingStarted;

                // if we are zerg, the buildingUnit now becomes nullptr since it's destroyed
                if (_opponentView.self()->getRace() == BWAPI::Races::Zerg)
                {
                    b.builderUnit = nullptr;
                    // if we are protoss, give the worker back to worker manager
                }
                else if (_opponentView.self()->getRace() == BWAPI::Races::Protoss)
                {    
					_workerManager.finishedWithWorker(b.builderUnit, currentFrame);
                    b.builderUnit = nullptr;
                }

                // put it in the under construction vector
                b.status = BuildingStatus::UnderConstruction;

                // free this space
                _buildingPlacer.freeTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());

                // only one building will match
                break;
            }
        }
    }
}

// STEP 5: IF WE ARE TERRAN, THIS MATTERS, SO: LOL
void BuildingManager::checkForDeadTerranBuilders() {}

// STEP 6: CHECK FOR COMPLETED BUILDINGS
void BuildingManager::checkForCompletedBuildings(int currentFrame)
{
    std::vector<Building> toRemove;

    // for each of our buildings under construction
    for (auto & b : _buildings)
    {
        if (b.status != BuildingStatus::UnderConstruction)
        {
            continue;       
        }

        // if the unit has completed
        if (b.buildingUnit->isCompleted())
        {
            // if we are terran, give the worker back to worker manager
            if (_opponentView.self()->getRace() == BWAPI::Races::Terran)
            {
                _workerManager.finishedWithWorker(b.builderUnit, currentFrame);
            }

            // remove this unit from the under construction vector
            toRemove.push_back(b);
        }
    }

    removeBuildings(toRemove);
}

// COMPLETED
bool BuildingManager::isEvolvedBuilding(BWAPI::UnitType type) 
{
    if (type == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
        type == BWAPI::UnitTypes::Zerg_Spore_Colony ||
        type == BWAPI::UnitTypes::Zerg_Lair ||
        type == BWAPI::UnitTypes::Zerg_Hive ||
        type == BWAPI::UnitTypes::Zerg_Greater_Spire)
    {
        return true;
    }

    return false;
}

// add a new building to be constructed
void BuildingManager::addBuildingTask(BWAPI::UnitType type, BWAPI::TilePosition desiredLocation)
{
    _reservedMinerals += type.mineralPrice();
    _reservedGas	     += type.gasPrice();

    Building b(type, desiredLocation);
    b.status = BuildingStatus::Unassigned;

    _buildings.push_back(b);
}

bool BuildingManager::isBuildingPositionExplored(const Building & b) const
{
    BWAPI::TilePosition tile = b.finalPosition;

    // for each tile where the building will be built
    for (int x=0; x<b.type.tileWidth(); ++x)
    {
        for (int y=0; y<b.type.tileHeight(); ++y)
        {
            if (!BWAPI::Broodwar->isExplored(tile.x + x,tile.y + y))
            {
                return false;
            }
        }
    }

    return true;
}

char BuildingManager::getBuildingWorkerCode(const Building & b) const
{
    return b.builderUnit == nullptr ? 'X' : 'W';
}

int BuildingManager::getReservedMinerals() 
{
    return _reservedMinerals;
}

int BuildingManager::getReservedGas() 
{
    return _reservedGas;
}

std::vector<BWAPI::UnitType> BuildingManager::buildingsQueued() const
{
    std::vector<BWAPI::UnitType> buildingsQueued;

    for (const auto & b : _buildings)
    {
        if (b.status == BuildingStatus::Unassigned || b.status == BuildingStatus::Assigned)
        {
            buildingsQueued.push_back(b.type);
        }
    }

    return buildingsQueued;
}

BWAPI::TilePosition BuildingManager::getBuildingLocation(const Building & b)
{
    int numPylons = _opponentView.self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Pylon);
    
    if (b.type.requiresPsi() && numPylons == 0)
    {
        return BWAPI::TilePositions::None;
    }

    if (b.type.isRefinery())
    {
        return _buildingPlacer.getRefineryPosition();
    }

    if (b.type.isResourceDepot())
    {
        // get the location 
        BWAPI::TilePosition tile = _bases.getNextExpansion(_opponentView.self());

        return tile;
    }

    // set the building padding specifically
    int distance = b.type == BWAPI::UnitTypes::Protoss_Photon_Cannon ? 0 : Config::Macro::BuildingSpacing;
    if (b.type == BWAPI::UnitTypes::Protoss_Pylon && (numPylons < 3))
    {
        distance = Config::Macro::PylonSpacing;
    }

    // get a position within our region
    return _buildingPlacer.getBuildLocationNear(b,distance,false);
}

void BuildingManager::removeBuildings(const std::vector<Building> & toRemove)
{
    for (auto & b : toRemove)
    {
        auto it = std::find(_buildings.begin(), _buildings.end(), b);

        if (it != _buildings.end())
        {
            _buildings.erase(it);
        }
    }
}