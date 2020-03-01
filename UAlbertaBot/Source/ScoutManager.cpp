#include "ScoutManager.h"
#include "ProductionManager.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "MicroManager.h"
#include "InformationManager.h"
#include "Micro.h"
#include "WorkerManager.h"
#include "MapTools.h"

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
    BWAPI::Broodwar->drawTextScreen(x, y+10, "GasSteal: %s", m_gasStealStatus.c_str());
}

void ScoutManager::moveScouts()
{
    if (!m_workerScout || !m_workerScout->exists() || !(m_workerScout->getHitPoints() > 0))
    {
        return;
    }

    const int scoutHP = m_workerScout->getHitPoints() + m_workerScout->getShields();

    gasSteal();

    // get the enemy base location, if we have one
    const BaseLocation * enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());

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

        for (auto startLocation : BWAPI::Broodwar->getStartLocations())
        {
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
                    m_scoutStatus = "Following perimeter";
                    Micro::SmartMove(m_workerScout, BWAPI::Position(enemyBaseLocation->getPosition()));
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
