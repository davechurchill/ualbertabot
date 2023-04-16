#include "Common.h"
#include "BuildingManager.h"
#include "Micro.h"
#include "ScoutManager.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "BuildingData.h"
#include "WorkerManager.h"
#include "BuildingPlacerManager.h"
#include "InformationManager.h"

using namespace UAlbertaBot;

BuildingManager::BuildingManager()
{

}

// gets called every frame from GameCommander
void BuildingManager::update()
{
    PROFILE_FUNCTION();

    validateWorkersAndBuildings();          // check to see if assigned workers have died en route or while constructing
    assignWorkersToUnassignedBuildings();   // assign workers to the unassigned buildings and label them 'planned'    
    constructAssignedBuildings();           // for each planned building, if the worker isn't constructing, send the command    
    checkForStartedConstruction();          // check to see if any buildings have started construction and update data structures    
    checkForDeadTerranBuilders();           // if we are terran and a building is under construction without a worker, assign a new one    
    checkForCompletedBuildings();           // check to see if any buildings have completed and update data structures
    
    rushDefence();


    drawBuildingInformation(200, 50);
    m_buildingPlacer.drawReservedTiles();
    //if (BWAPI::Broodwar->getFrameCount() % 96 == 0)
    //{
    //    if (m_buildings.size())
    //        
    //        << "------------------------------------" << std::endl;
    //    for (auto& b : m_buildings)
    //    {
    //        if (b.builderUnit != nullptr)
    //            std::cout << "Worker_ID: " << b.builderUnit->getID() << "\tBuilding_name: " << b.type.getName() << std::endl;
    //        else
    //            std::cout << "Worker_ID: " << "nullprt" << "\tBuilding_name: " << b.type.getName() << std::endl;
    //    }
    //}
}

bool BuildingManager::isBeingBuilt(BWAPI::UnitType type)
{
    for (auto & b : m_buildings)
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
    PROFILE_FUNCTION();

    // TODO: if a terran worker dies while constructing and its building
    //       is under construction, place unit back into buildingsNeedingBuilders

    std::vector<Building> toRemove;

    for (auto & b : m_buildings)
    {
        if (b.status != BuildingStatus::UnderConstruction)
        {
            continue;
        }
        if (b.buildingUnit == nullptr || !b.buildingUnit->getType().isBuilding() || b.buildingUnit->getHitPoints() <= 0 || !b.buildingUnit->exists())
        {
            toRemove.push_back(b);

            /// change worker status from building (@WorkerJob 2) to idle (@WorkerJob 4)
            if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran && b.builderUnit->exists())
            {
                if (b.isGasSteal)
                {
                    Global::Scout().setWorkerScout(b.builderUnit);
                }
                // otherwise tell the worker manager we're finished with this unit
                else
                {
                    Global::Workers().finishedWithWorker(b.builderUnit);
                }
            }
        }
    }

    removeBuildings(toRemove);
}

// STEP 2: ASSIGN WORKERS TO BUILDINGS WITHOUT THEM
void BuildingManager::assignWorkersToUnassignedBuildings()
{
    PROFILE_FUNCTION();

    // for each building that doesn't have a builder, assign one
    for (Building & b : m_buildings)
    {
        if (b.status != BuildingStatus::Unassigned)
        {
            continue;
        }

        if (m_debugMode) { BWAPI::Broodwar->printf("Assigning Worker To: %s",b.type.getName().c_str()); }

        // grab a worker unit from WorkerManager which is closest to this final position
        BWAPI::Unit workerToAssign = Global::Workers().getBuilder(b);

        if (workerToAssign)
        {
            //BWAPI::Broodwar->printf("VALID WORKER BEING ASSIGNED: %d", workerToAssign->getID());

            // TODO: special case of terran building whose worker died mid construction
            //       send the right click command to the buildingUnit to resume construction
            //		 skip the buildingsAssigned step and push it back into buildingsUnderConstruction

            b.builderUnit = workerToAssign;

            // if building has been started but its not finished and worker is killed 
            // assigne another worker by right click on it
            if (b.buildingUnit)
            {
                Micro::SmartRightClick(b.builderUnit, b.buildingUnit);
                b.status = BuildingStatus::UnderConstruction;
                continue;
            }

            BWAPI::TilePosition testLocation = getBuildingLocation(b);
            if (!testLocation.isValid())
            {
                continue;
            }

            b.finalPosition = testLocation;
            
            // reserve this building's space
            m_buildingPlacer.reserveTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());
            b.status = BuildingStatus::Assigned;
            
        }
    }
}

// STEP 3: ISSUE CONSTRUCTION ORDERS TO ASSIGN BUILDINGS AS NEEDED
void BuildingManager::constructAssignedBuildings()
{
    PROFILE_FUNCTION();

    for (auto & b : m_buildings)
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
                Micro::SmartMove(b.builderUnit,BWAPI::Position(b.finalPosition));
            }
            // if this is not the first time we've sent this guy to build this
            // it must be the case that something was in the way of building
            else if (b.buildCommandGiven)
            {
                // tell worker manager the unit we had is not needed now, since we might not be able
                // to get a valid location soon enough
                Global::Workers().finishedWithWorker(b.builderUnit);

                // free the previous location in reserved
                m_buildingPlacer.freeTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());

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
void BuildingManager::checkForStartedConstruction()
{
    PROFILE_FUNCTION();

    // for each building unit which is being constructed
    for (auto & buildingStarted : BWAPI::Broodwar->self()->getUnits())
    {
        // filter out units which aren't buildings under construction
        if (!buildingStarted->getType().isBuilding() || !buildingStarted->isBeingConstructed())
        {
            continue;
        }

        // check all our building status objects to see if we have a match and if we do, update it
        for (auto & b : m_buildings)
        {
            if (b.status != BuildingStatus::Assigned)
            {
                continue;
            }
        
            // check if the positions match
            if (b.finalPosition == buildingStarted->getTilePosition())
            {
                // the resources should now be spent, so unreserve them
                m_reservedMinerals -= buildingStarted->getType().mineralPrice();
                m_reservedGas      -= buildingStarted->getType().gasPrice();

                // flag it as started and set the buildingUnit
                b.underConstruction = true;
                b.buildingUnit = buildingStarted;

                // if we are zerg, the buildingUnit now becomes nullptr since it's destroyed
                if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
                {
                    b.builderUnit = nullptr;
                    // if we are protoss, give the worker back to worker manager
                }
                else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
                {
                    // if this was the gas steal unit then it's the scout worker so give it back to the scout manager
                    if (b.isGasSteal)
                    {
                        Global::Scout().setWorkerScout(b.builderUnit);
                    }
                    // otherwise tell the worker manager we're finished with this unit
                    else
                    {
                        Global::Workers().finishedWithWorker(b.builderUnit);
                    }

                    b.builderUnit = nullptr;
                }

                // put it in the under construction vector
                b.status = BuildingStatus::UnderConstruction;

                // free this space
                m_buildingPlacer.freeTiles(b.finalPosition,b.type.tileWidth(),b.type.tileHeight());

                // only one building will match
                break;
            }
        }
    }
}

// STEP 5: IF WE ARE TERRAN, THIS MATTERS, SO: LOL
// CHCEK IF ASSIGNED WORKER IS STILL ALIVE
void BuildingManager::checkForDeadTerranBuilders() 
{
    for (auto & b : m_buildings)
    {
        // for each building under construction chcek status assigned worker
        if (b.status == BuildingStatus::UnderConstruction && !b.builderUnit->exists())
        {
            // constructig worker was killed, setting back to status @Unassigned
            b.status = BuildingStatus::Unassigned;
        }
    }

}

// STEP 6: CHECK FOR COMPLETED BUILDINGS
void BuildingManager::checkForCompletedBuildings()
{
    PROFILE_FUNCTION();

    std::vector<Building> toRemove;

    // for each of our buildings under construction
    for (auto & b : m_buildings)
    {
        if (b.status != BuildingStatus::UnderConstruction)
        {
            continue;       
        }

        // if the unit has completed
        if (b.buildingUnit->isCompleted())
        {
            // if we are terran, give the worker back to worker manager
            if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
            {
                Global::Workers().finishedWithWorker(b.builderUnit);
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
void BuildingManager::addBuildingTask(BWAPI::UnitType type, BWAPI::TilePosition desiredLocation, bool isGasSteal)
{
    m_reservedMinerals   += type.mineralPrice();
    m_reservedGas	     += type.gasPrice();

    Building b(type, desiredLocation);
    b.isGasSteal = isGasSteal;
    b.status = BuildingStatus::Unassigned;

    m_buildings.push_back(b);
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
    return m_reservedMinerals;
}

int BuildingManager::getReservedGas() 
{
    return m_reservedGas;
}

void BuildingManager::drawBuildingInformation(int x,int y)
{
    if (!Config::Debug::DrawBuildingInfo)
    {
        return;
    }

    PROFILE_FUNCTION();

    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        BWAPI::Broodwar->drawTextMap(unit->getPosition().x,unit->getPosition().y+5,"\x07%d",unit->getID());
    }

    BWAPI::Broodwar->drawTextScreen(x,y,"\x04 Building Information:");
    BWAPI::Broodwar->drawTextScreen(x,y+20,"\x04 Name");
    BWAPI::Broodwar->drawTextScreen(x+150,y+20,"\x04 State");

    int yspace = 0;

    for (const auto & b : m_buildings)
    {
        if (b.status == BuildingStatus::Unassigned)
        {
            BWAPI::Broodwar->drawTextScreen(x,y+40+((yspace)*10),"\x03 %s",b.type.getName().c_str());
            BWAPI::Broodwar->drawTextScreen(x+150,y+40+((yspace++)*10),"\x03 Need %c",getBuildingWorkerCode(b));
        }
        else if (b.status == BuildingStatus::Assigned)
        {
            BWAPI::Broodwar->drawTextScreen(x,y+40+((yspace)*10),"\x03 %s %d",b.type.getName().c_str(),b.builderUnit->getID());
            BWAPI::Broodwar->drawTextScreen(x+150,y+40+((yspace++)*10),"\x03 A %c (%d,%d)",getBuildingWorkerCode(b),b.finalPosition.x,b.finalPosition.y);

            int x1 = b.finalPosition.x*32;
            int y1 = b.finalPosition.y*32;
            int x2 = (b.finalPosition.x + b.type.tileWidth())*32;
            int y2 = (b.finalPosition.y + b.type.tileHeight())*32;

            BWAPI::Broodwar->drawLineMap(b.builderUnit->getPosition().x,b.builderUnit->getPosition().y,(x1+x2)/2,(y1+y2)/2,BWAPI::Colors::Orange);
            BWAPI::Broodwar->drawBoxMap(x1,y1,x2,y2,BWAPI::Colors::Red,false);
        }
        else if (b.status == BuildingStatus::UnderConstruction)
        {
            BWAPI::Broodwar->drawTextScreen(x,y+40+((yspace)*10),"\x03 %s %d",b.type.getName().c_str(),b.buildingUnit->getID());
            BWAPI::Broodwar->drawTextScreen(x+150,y+40+((yspace++)*10),"\x03 Const %c",getBuildingWorkerCode(b));
        }
    }
}

std::vector<BWAPI::UnitType> BuildingManager::buildingsQueued()
{
    std::vector<BWAPI::UnitType> buildingsQueued;

    for (const auto & b : m_buildings)
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
    int numPylons = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Pylon);

    if (b.isGasSteal)
    {
		auto enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());
        UAB_ASSERT(enemyBaseLocation,"Should have enemy base location before attempting gas steal");
        UAB_ASSERT(enemyBaseLocation->getGeysers().size() > 0,"Should have spotted an enemy geyser");

        for (auto & unit : enemyBaseLocation->getGeysers())
        {
            BWAPI::TilePosition tp(unit->getInitialTilePosition());
            return tp;
        }
    }

    if (b.type.requiresPsi() && numPylons == 0)
    {
        return BWAPI::TilePositions::None;
    }

    if (b.type.isRefinery())
    {
        return m_buildingPlacer.getRefineryPosition();
    }

    if (b.type.isResourceDepot())
    {
        // get the location 
		auto tile = Global::Bases().getNextExpansion(BWAPI::Broodwar->self());

        return tile;
    }

    // set the building padding specifically
    int distance = b.type == BWAPI::UnitTypes::Protoss_Photon_Cannon ? 0 : Config::Macro::BuildingSpacing;
    if (b.type == BWAPI::UnitTypes::Protoss_Pylon && (numPylons < 3))
    {
        distance = Config::Macro::PylonSpacing;
    }

    // get a position within our region
    return m_buildingPlacer.getBuildLocationNear(b,distance,false);
}

void BuildingManager::removeBuildings(const std::vector<Building> & toRemove)
{
    for (auto & b : toRemove)
    {
        auto & it = std::find(m_buildings.begin(), m_buildings.end(), b);

        if (it != m_buildings.end())
        {
            m_buildings.erase(it);
        }
    }
}




void BuildingManager::rushDefence()
{
    if (Global::Info().getRushInfo())
    {

    }
}