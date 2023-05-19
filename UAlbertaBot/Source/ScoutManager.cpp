#include "ScoutManager.h"
#include "ProductionManager.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "MicroManager.h"
#include "InformationManager.h"
#include "Micro.h"
#include "WorkerManager.h"
#include "MapTools.h"
#include "bwem.h"

using namespace UAlbertaBot;

ScoutManager::ScoutManager()
{
}

void ScoutManager::update()
{
    PROFILE_FUNCTION();

    if (!Config::Modules::UsingScoutManager)
    {
        return; 
    }

    //rushWarning();
    moveScouts();
    drawScoutInformation(200, 320);
}

void ScoutManager::setWorkerScout(BWAPI::Unit unit)
{
    // if we have a previous worker scout, release it back to the worker manager
    if (m_workerScout)
    {
        Global::Workers().finishedWithWorker(m_workerScout);
    }

    m_workerScout = unit;
    Global::Workers().setScoutWorker(m_workerScout);
}

void ScoutManager::drawScoutInformation(int x, int y)
{
    if (!Config::Debug::DrawScoutInfo)
    {
        return;
    }

    BWAPI::Broodwar->drawTextScreen(x, y, "ScoutInfo: %s", m_scoutStatus.c_str());
    BWAPI::Broodwar->drawTextScreen(x, y + 10, "GasSteal: %s", m_gasStealStatus.c_str());
}

void ScoutManager::moveScouts()
{
    const BWEM::Map& mapa = BWEM::Map::Instance();

    if (!m_workerScout || !m_workerScout->exists() || !(m_workerScout->getHitPoints() > 0))
    {
        return;
    }

    const int scoutHP = m_workerScout->getHitPoints() + m_workerScout->getShields();

    gasSteal();

    // get the enemy base location, if we have one
    const BaseLocation* enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());
    const int scoutDistanceThreshold = 30;

    if (m_workerScout->isCarryingGas())
    {
        BWAPI::Broodwar->drawCircleMap(m_workerScout->getPosition(), 10, BWAPI::Colors::Purple, true);
    }

    // if we initiated a gas steal and the worker isn't idle, 
    const bool finishedConstructingGasSteal = m_workerScout->isIdle() || m_workerScout->isCarryingGas();
    if (!m_gasStealFinished && m_didGasSteal && !finishedConstructingGasSteal)
    {
        return;
    }
    // check to see if the gas steal is completed
    else if (m_didGasSteal && finishedConstructingGasSteal)
    {
        m_gasStealFinished = true;
    }

    // for each start location in the level
    if (!enemyBaseLocation)
    {
        m_scoutStatus = "Enemy base unknown, exploring";

        for (const BWAPI::TilePosition startLocation : BWAPI::Broodwar->getStartLocations())
        {
            if (scoutHP < m_previousScoutHP)
            {
                Global::Info().setRushInfo(true);
            }
            // if we haven't explored it yet, explore it
            if (!BWAPI::Broodwar->isExplored(startLocation))
            {
                Micro::SmartMove(m_workerScout, BWAPI::Position(startLocation));
                BWAPI::Broodwar->drawCircleMap(BWAPI::Position(startLocation), 32, BWAPI::Colors::Purple);
                return;
            }
        }
    }

    // if we know where the enemy region is and where our scout is
    if (enemyBaseLocation)
    {
        const int scoutDistanceToEnemy = Global::Map().getGroundDistance(m_workerScout->getPosition(), enemyBaseLocation->getPosition());
        const bool scoutInRangeOfenemy = scoutDistanceToEnemy <= scoutDistanceThreshold;

        // we only care if the scout is under attack within the enemy region
        // this ignores if their scout worker attacks it on the way to their base
        if (scoutHP < m_previousScoutHP)
        {
            if (m_workerScout->getClosestUnit()->getType() == BWAPI::UnitTypes::Zerg_Zergling)
            {
                Global::Info().setRushInfo(true);
            }
            m_scoutUnderAttack = true;
        }

        if (!m_workerScout->isUnderAttack() && !enemyWorkerInRadius())
        {
            m_scoutUnderAttack = false;
        }

        // if the scout is in the enemy region
        if (scoutInRangeOfenemy)
        {
            // get the closest enemy worker
            BWAPI::Unit closestWorker = closestEnemyWorker();

            // if the worker scout is not under attack
            if (!m_scoutUnderAttack)
            {
                // if there is a worker nearby, harass it
                if (Config::Strategy::ScoutHarassEnemy && (!Config::Strategy::GasStealWithScout || m_gasStealFinished) && closestWorker && (m_workerScout->getDistance(closestWorker) < 800))
                {
                    m_scoutStatus = "Harass enemy worker";
                    Micro::SmartAttackUnit(m_workerScout, closestWorker);
                }
                // otherwise keep moving to the enemy region
                else
                {
                    if (m_scoutStatus == "Enemy base unknown, exploring" || m_scoutStatus == "Enemy region known, going there")
                        m_scoutStatus = "Enemy Base";
                    followPerimeter(enemyBaseLocation);
                    //Micro::SmartMove(m_workerScout, BWAPI::Position(enemyBaseLocation->getPosition()));
                }

            }
            // if the worker scout is under attack
            else
            {
                m_scoutStatus = "Under attack inside, fleeing";
                fleeScout();
            }
        }
        // if the scout is not in the enemy region
        else if (m_scoutUnderAttack)
        {
            m_scoutStatus = "Under attack inside, fleeing";

            fleeScout();
        }
        else
        {
            m_scoutStatus = "Enemy region known, going there";

            // move to the enemy region
            Micro::SmartMove(m_workerScout, BWAPI::Position(enemyBaseLocation->getPosition()));
        }

    }

    m_previousScoutHP = scoutHP;
}

void ScoutManager::fleeScout()
{
    const BWAPI::Position fleeTo = getFleePosition();

    if (Config::Debug::DrawScoutInfo)
    {
        BWAPI::Broodwar->drawCircleMap(fleeTo, 5, BWAPI::Colors::Red, true);
    }

    Micro::SmartMove(m_workerScout, fleeTo);
}

void ScoutManager::gasSteal()
{
    if (!Config::Strategy::GasStealWithScout)
    {
        m_gasStealStatus = "Not using gas steal";
        return;
    }

    if (m_didGasSteal)
    {
        return;
    }

    if (!m_workerScout)
    {
        m_gasStealStatus = "No worker scout";
        return;
    }

    auto enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());
    if (!enemyBaseLocation)
    {
        m_gasStealStatus = "No enemy base location found";
        return;
    }

    const BWAPI::Unit enemyGeyser = getEnemyGeyser();
    if (!enemyGeyser)
    {
        m_gasStealStatus = "No enemy geyser found";
        false;
    }

    if (!m_didGasSteal)
    {
        Global::Production().queueGasSteal();
        m_didGasSteal = true;
        Micro::SmartMove(m_workerScout, enemyGeyser->getPosition());
        m_gasStealStatus = "Did Gas Steal";
    }
}

BWAPI::Unit ScoutManager::closestEnemyWorker()
{
    BWAPI::Unit enemyWorker = nullptr;
    BWAPI::Unit geyser      = getEnemyGeyser();
    double      maxDist     = 0;

    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (unit->getType().isWorker() && unit->isConstructing())
        {
            return unit;
        }
    }

    // for each enemy worker
    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (unit->getType().isWorker())
        {
            const double dist = unit->getDistance(geyser);

            if (dist < 800 && dist > maxDist)
            {
                maxDist = dist;
                enemyWorker = unit;
            }
        }
    }

    return enemyWorker;
}

BWAPI::Unit ScoutManager::getEnemyGeyser()
{
    BWAPI::Unit geyser = nullptr;
    const auto enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());

    for (auto & unit : enemyBaseLocation->getGeysers())
    {
        geyser = unit;
    }

    return geyser;
}

bool ScoutManager::enemyWorkerInRadius()
{
    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (unit->getType().isWorker() && (unit->getDistance(m_workerScout) < 300))
        {
            return true;
        }
    }

    return false;
}

bool ScoutManager::immediateThreat()
{
    BWAPI::Unitset enemyAttackingWorkers;
    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (unit->getType().isWorker() && unit->isAttacking())
        {
            enemyAttackingWorkers.insert(unit);
        }
    }

    if (m_workerScout->isUnderAttack())
    {
        return true;
    }

    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        const double dist = unit->getDistance(m_workerScout);
        const double range = unit->getType().groundWeapon().maxRange();

        if (unit->getType().canAttack() && !unit->getType().isWorker() && (dist <= range + 32))
        {
            return true;
        }
    }

    return false;
}

BWAPI::Position ScoutManager::getFleePosition()
{
    return BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
}

void ScoutManager::followPerimeter(const BaseLocation * enemyBaseLocation)
{
    // Get the BWEM map instance
    const BWEM::Map& mapa = BWEM::Map::Instance();

    // Find the closest choke point to the enemy starting location
    const BWEM::ChokePoint* closestChokepoint = nullptr;
    int closestChokeDist = INT_MAX;
    
    const auto enemyArea = mapa.GetArea(enemyBaseLocation->getDepotPosition());

    BWAPI::Position cen = BWAPI::Position(int((BWAPI::Position(enemyArea->TopLeft()).x + BWAPI::Position(enemyArea->BottomRight()).x) / 2),
        int((BWAPI::Position(enemyArea->TopLeft()).y + BWAPI::Position(enemyArea->BottomRight()).y) / 2));
    BWAPI::Position but = BWAPI::Position(cen.x, cen.y - 1000);
    for (const auto& choke : (mapa.GetArea(enemyBaseLocation->getDepotPosition()))->ChokePoints())
    {
        const int dist = enemyBaseLocation->getGroundDistance(BWAPI::TilePosition(choke->Center()));
        if (dist < closestChokeDist)
        {
            closestChokeDist = dist;
            closestChokepoint = choke;
        }
    }

    const int dist = int(BWAPI::Position(enemyArea->TopLeft()).getDistance(BWAPI::Position(enemyArea->BottomRight())));

    const BWAPI::Position pointTL = BWAPI::Position(cen.x - int(dist / 6), cen.y - int(dist / 6));
    const BWAPI::Position pointTR = BWAPI::Position(cen.x + int(dist / 6), cen.y - int(dist / 6));
    const BWAPI::Position pointBL = BWAPI::Position(cen.x - int(dist / 6), cen.y + int(dist / 6));
    const BWAPI::Position pointBR = BWAPI::Position(cen.x + int(dist / 6), cen.y + int(dist / 6));
 
    BWAPI::Broodwar->drawBoxMap(pointTL, pointBR, BWAPI::Colors::Green);


    const BWAPI::Position scoutPos = m_workerScout->getPosition();
    const BWAPI::Position enemyBasePos = enemyBaseLocation->getPosition();

    if (m_scoutStatus == "Enemy Base" && scoutPos.getDistance(enemyBasePos) < 75)
    {
        rushWarning();
        auto en = BWAPI::Broodwar->enemy()->getRace();
        m_scoutStatus = "Enemy TL";
    }
    else if (m_scoutStatus == "Enemy TL" && scoutPos.getDistance(pointTL) < 100)
    {
        rushWarning();
        m_scoutStatus = "Enemy TR";
    }
    else if (m_scoutStatus == "Enemy TR" && scoutPos.getDistance(pointTR) < 100)
    {
        rushWarning();
        m_scoutStatus = "Enemy BR";
    }
    else if (m_scoutStatus == "Enemy BR" && scoutPos.getDistance(pointBR) < 100)
    {
        rushWarning();
        m_scoutStatus = "Enemy BL";
    }
    else if (m_scoutStatus == "Enemy BL" && scoutPos.getDistance(pointBL) < 100)
    {
        rushWarning();
        m_scoutStatus = "Enemy TL";
    }

    if (m_scoutStatus == "Enemy Base")
    {
        Micro::SmartMove(m_workerScout, enemyBasePos);
    }
    else if (m_scoutStatus == "Enemy TL")
    {
        Micro::SmartMove(m_workerScout, pointTL);
    }
    else if (m_scoutStatus == "Enemy TR")
    {
        Micro::SmartMove(m_workerScout, pointTR);
    }
    else if (m_scoutStatus == "Enemy BR")
    {
        Micro::SmartMove(m_workerScout, pointBR);
    }
    else if (m_scoutStatus == "Enemy BL")
    {
        lap++;
        Micro::SmartMove(m_workerScout, pointBL);
    }
}

void ScoutManager::rushWarning()
{
    int numOfBuildings = 0;
    int expo = 0;
    int gate = 0;

    for (auto ui : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (ui->getType().isBuilding())
        {
            numOfBuildings++;
            if (ui->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
                Global::Info().setDtRushInfo(true);
            if (ui->getType() == BWAPI::UnitTypes::Protoss_Gateway)
                gate++;
            if (ui->getType() == BWAPI::UnitTypes::Protoss_Forge || ui->getType() == BWAPI::UnitTypes::Protoss_Photon_Cannon)
                expo++;
        }

        if (m_rushFalse && (ui->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool || ui->getType() == BWAPI::UnitTypes::Protoss_Gateway))
        {
            Global::Info().setRushInfo(true);
        }
    }
    if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg && numOfBuildings > 2)
    {
        m_rushFalse = false;
        Global::Info().setRushInfo(false);
    }
    if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss && (expo > 1 && gate < 1 || lap != 0 && gate == 0 && !Global::Info().getRushInfo()))
    {
        Config::Micro::UseSparcraftSimulation = false;
        Global::Info().setExpandInfo(true);
    }
}
