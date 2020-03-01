#include "Squad.h"
#include "UnitUtil.h"
#include "Global.h"
#include "WorkerManager.h"
#include "UnitData.h"
#include "MapTools.h"
#include "StrategyManager.h"
#include "CombatSimulation.h"
#include "InformationManager.h"

using namespace UAlbertaBot;

Squad::Squad()
{
    
}

Squad::Squad(const std::string & name, SquadOrder order, size_t priority)
    : m_name(name)
    , m_order(order)
    , m_lastRetreatSwitch(0)
    , m_lastRetreatSwitchVal(false)
    , m_priority(priority)
{
}

Squad::~Squad()
{
    clear();
}

void Squad::update()
{
    // update all necessary unit information within this squad
    updateUnits();

    // determine whether or not we should regroup
    bool needToRegroup = needsToRegroup();

    // draw some debug info
    if (Config::Debug::DrawSquadInfo && m_order.getType() == SquadOrderTypes::Attack)
    {
        BWAPI::Broodwar->drawTextScreen(200, 350, "%s", m_regroupStatus.c_str());

        BWAPI::Unit closest = unitClosestToEnemy();
    }

    // if we do need to regroup, do it
    if (needToRegroup)
    {
        BWAPI::Position regroupPosition = calcRegroupPosition();

        if (Config::Debug::DrawCombatSimulationInfo)
        {
            BWAPI::Broodwar->drawTextScreen(200, 150, "REGROUP");
        }

        BWAPI::Broodwar->drawCircleMap(regroupPosition.x, regroupPosition.y, 30, BWAPI::Colors::Purple, true);

        m_meleeManager.regroup(regroupPosition);
        m_rangedManager.regroup(regroupPosition);
        m_tankManager.regroup(regroupPosition);
        m_medicManager.regroup(regroupPosition);
    }
    else // otherwise, execute micro
    {
        m_meleeManager.execute(m_order);
        m_rangedManager.execute(m_order);
        m_tankManager.execute(m_order);
        m_medicManager.execute(m_order);
        m_transportManager.update();

        m_detectorManager.setUnitClosestToEnemy(unitClosestToEnemy());
        m_detectorManager.execute(m_order);
    }
}

bool Squad::isEmpty() const
{
    return m_units.empty();
}

size_t Squad::getPriority() const
{
    return m_priority;
}

void Squad::setPriority(const size_t & priority)
{
    m_priority = priority;
}

void Squad::updateUnits()
{
    setAllUnits();
    setNearEnemyUnits();
    addUnitsToMicroManagers();
}

void Squad::setAllUnits()
{
    // clean up the m_units vector just in case one of them died
    BWAPI::Unitset goodUnits;
    for (auto & unit : m_units)
    {
        if (unit->isCompleted() &&
            unit->getHitPoints() > 0 &&
            unit->exists() &&
            unit->getPosition().isValid() &&
            unit->getType() != BWAPI::UnitTypes::Unknown)
        {
            goodUnits.insert(unit);
        }
    }
    m_units = goodUnits;
}

void Squad::setNearEnemyUnits()
{
    m_nearEnemy.clear();
    for (auto & unit : m_units)
    {
        int x = unit->getPosition().x;
        int y = unit->getPosition().y;

        int left = unit->getType().dimensionLeft();
        int right = unit->getType().dimensionRight();
        int top = unit->getType().dimensionUp();
        int bottom = unit->getType().dimensionDown();

        m_nearEnemy[unit] = unitNearEnemy(unit);
        if (m_nearEnemy[unit])
        {
            if (Config::Debug::DrawSquadInfo) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Config::Debug::ColorUnitNearEnemy);
        }
        else
        {
            if (Config::Debug::DrawSquadInfo) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Config::Debug::ColorUnitNotNearEnemy);
        }
    }
}

void Squad::addUnitsToMicroManagers()
{
    BWAPI::Unitset meleeUnits;
    BWAPI::Unitset rangedUnits;
    BWAPI::Unitset detectorUnits;
    BWAPI::Unitset transportUnits;
    BWAPI::Unitset tankUnits;
    BWAPI::Unitset medicUnits;

    // add m_units to micro managers
    for (auto & unit : m_units)
    {
        if (unit->isCompleted() && unit->getHitPoints() > 0 && unit->exists())
        {
            // select dector m_units
            if (unit->getType() == BWAPI::UnitTypes::Terran_Medic)
            {
                medicUnits.insert(unit);
            }
            else if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
            {
                tankUnits.insert(unit);
            }
            else if (unit->getType().isDetector() && !unit->getType().isBuilding())
            {
                detectorUnits.insert(unit);
            }
            // select transport m_units
            else if (unit->getType() == BWAPI::UnitTypes::Protoss_Shuttle || unit->getType() == BWAPI::UnitTypes::Terran_Dropship)
            {
                transportUnits.insert(unit);
            }
            // select ranged m_units
            else if ((unit->getType().groundWeapon().maxRange() > 32) || (unit->getType() == BWAPI::UnitTypes::Protoss_Reaver) || (unit->getType() == BWAPI::UnitTypes::Zerg_Scourge))
            {
                rangedUnits.insert(unit);
            }
            // select melee m_units
            else if (unit->getType().groundWeapon().maxRange() <= 32)
            {
                meleeUnits.insert(unit);
            }
        }
    }

    m_meleeManager.setUnits(meleeUnits);
    m_rangedManager.setUnits(rangedUnits);
    m_detectorManager.setUnits(detectorUnits);
    m_transportManager.setUnits(transportUnits);
    m_tankManager.setUnits(tankUnits);
    m_medicManager.setUnits(medicUnits);
}

// calculates whether or not to regroup
bool Squad::needsToRegroup()
{
    if (!Config::Micro::UseSparcraftSimulation)
    {
        return false;
    }

    // if we are not attacking, never regroup
    if (m_units.empty() || (m_order.getType() != SquadOrderTypes::Attack))
    {
        m_regroupStatus = std::string("\x04 No combat units available");
        return false;
    }

    BWAPI::Unit unitClosest = unitClosestToEnemy();

    if (!unitClosest)
    {
        m_regroupStatus = std::string("\x04 No closest unit");
        return false;
    }

    // if none of our units are in attack range of any enemy units, don't retreat
    std::vector<UnitInfo> enemyCombatUnits;
    const auto & enemyUnitInfo = Global::Info().getUnitInfo(BWAPI::Broodwar->enemy());

    bool anyInRange = false;
    for (const auto & eui : enemyUnitInfo)
    {
        bool inRange = false;
        for (const auto & u : m_units)
        {
            int range = UnitUtil::GetAttackRange(eui.second.type, u->getType());

            if (range + 128 >= eui.second.lastPosition.getDistance(u->getPosition()))
            {
                inRange = true;
                break;
            }
        }

        if (inRange)
        {
            anyInRange = true;
            break;
        }
    }

    if (!anyInRange)
    {
        m_regroupStatus = std::string("\x04 No enemy units in attack range");
        return false;
    }

    SparCraft::ScoreType score = 0;

    //do the SparCraft Simulation!
    CombatSimulation sim;

    sim.setCombatUnits(unitClosest->getPosition(), Config::Micro::CombatRegroupRadius);
    score = sim.simulateCombat();

    // if we are DT rushing and we haven't lost a DT yet, no retreat!
    if (Config::Strategy::StrategyName == "Protoss_DTRush" && (BWAPI::Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar) == 0))
    {
        m_regroupStatus = std::string("\x04 DARK TEMPLAR HOOOOO!");
        return false;
    }

    bool retreat = score < 0;
    int switchTime = 100;
    bool waiting = false;

    // we should not attack unless 5 seconds have passed since a retreat
    if (retreat != m_lastRetreatSwitchVal)
    {
        if (!retreat && (BWAPI::Broodwar->getFrameCount() - m_lastRetreatSwitch < switchTime))
        {
            waiting = true;
            retreat = m_lastRetreatSwitchVal;
        }
        else
        {
            waiting = false;
            m_lastRetreatSwitch = BWAPI::Broodwar->getFrameCount();
            m_lastRetreatSwitchVal = retreat;
        }
    }

    if (retreat)
    {
        m_regroupStatus = std::string("\x04 Retreat - simulation predicts defeat");
    }
    else
    {
        m_regroupStatus = std::string("\x04 Attack - simulation predicts success");
    }

    return retreat;
}

void Squad::setSquadOrder(const SquadOrder & so)
{
    m_order = so;
}

bool Squad::containsUnit(BWAPI::Unit u) const
{
    return m_units.contains(u);
}

void Squad::clear()
{
    for (auto & unit : getUnits())
    {
        if (unit->getType().isWorker())
        {
            Global::Workers().finishedWithWorker(unit);
        }
    }

    m_units.clear();
}

bool Squad::unitNearEnemy(BWAPI::Unit unit)
{
    assert(unit);

    BWAPI::Unitset enemyNear;

    Global::Map().getUnits(enemyNear, unit->getPosition(), 400, false, true);

    return enemyNear.size() > 0;
}

BWAPI::Position Squad::calcCenter()
{
    if (m_units.empty())
    {
        if (Config::Debug::DrawSquadInfo)
        {
            BWAPI::Broodwar->printf("Squad::calcCenter() called on empty squad");
        }
        return BWAPI::Position(0, 0);
    }

    BWAPI::Position accum(0, 0);
    for (auto & unit : m_units)
    {
        accum += unit->getPosition();
    }
    return BWAPI::Position(accum.x / m_units.size(), accum.y / m_units.size());
}

BWAPI::Position Squad::calcRegroupPosition()
{
    BWAPI::Position regroup(0, 0);

    int minDist = 100000;

    for (auto & unit : m_units)
    {
        if (!m_nearEnemy[unit])
        {
            int dist = unit->getDistance(m_order.getPosition());
            if (dist < minDist)
            {
                minDist = dist;
                regroup = unit->getPosition();
            }
        }
    }

    if (regroup == BWAPI::Position(0, 0))
    {
        return BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
    }
    else
    {
        return regroup;
    }
}

BWAPI::Unit Squad::unitClosestToEnemy()
{
    BWAPI::Unit closest = nullptr;
    int closestDist = 100000;

    for (auto & unit : m_units)
    {
        if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
        {
            continue;
        }

        // the distance to the order position
        int dist = Global::Map().getGroundDistance(unit->getPosition(), m_order.getPosition());

        if (dist != -1 && (!closest || dist < closestDist))
        {
            closest = unit;
            closestDist = dist;
        }
    }

    if (!closest)
    {
        for (auto & unit : m_units)
        {
            if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
            {
                continue;
            }

            // the distance to the order position
            int dist = unit->getDistance(BWAPI::Position(BWAPI::Broodwar->enemy()->getStartLocation()));

            if (dist != -1 && (!closest || dist < closestDist))
            {
                closest = unit;
                closestDist = dist;
            }
        }
    }

    return closest;
}

int Squad::squadUnitsNear(BWAPI::Position p)
{
    int numUnits = 0;

    for (auto & unit : m_units)
    {
        if (unit->getDistance(p) < 600)
        {
            numUnits++;
        }
    }

    return numUnits;
}

const BWAPI::Unitset & Squad::getUnits() const
{
    return m_units;
}

const SquadOrder & Squad::getSquadOrder()	const
{
    return m_order;
}

void Squad::addUnit(BWAPI::Unit u)
{
    m_units.insert(u);
}

void Squad::removeUnit(BWAPI::Unit u)
{
    m_units.erase(u);
}

const std::string & Squad::getName() const
{
    return m_name;
}