#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "SparCraftAssert.h"

using namespace SparCraft;

GameState::GameState()
	: _map(NULL)
	, _currentTime(0)
{
    _numMovements[0] = 0;
    _numMovements[1] = 0;
}

void GameState::doAction(const Action & action)
{
	Unit & ourUnit		= _getUnitByID(action.getID());
	size_t player		= ourUnit.getPlayerID();
	size_t enemyPlayer  = getEnemy(player);
    TimeType gameTime   = getTime();

    SPARCRAFT_ASSERT(ourUnit.firstTimeFree() == getTime(), "Trying to take an action at a different frame than a unit is ready");

	if (action.type() == ActionTypes::ATTACK)
	{
		Unit & enemyUnit = _getUnitByID(action.getTargetID());
			
		// attack the unit
		ourUnit.attack(action, enemyUnit, getTime());
			
		// enemy unit takes damage if it is alive
		if (enemyUnit.isAlive())
		{				
			enemyUnit.takeAttack(ourUnit);

            // check to see if enemy unit died
	        if (!enemyUnit.isAlive())
	        {
		        // if it died, remove it
                _unitData.killUnit(enemyUnit.getID());
	        }
		}			    
	}
	else if (action.type() == ActionTypes::MOVE)
	{
		_numMovements[player]++;

        if (_map.get() == nullptr)
        {
            ourUnit.move(action, getTime());
        }
        else 
        {
            int pX = std::max(action.pos().x(), ourUnit.type().dimensionLeft());
            int pY = std::max(action.pos().y(), ourUnit.type().dimensionUp());
            pX = std::min(pX, (int)_map->getPixelWidth() - ourUnit.type().dimensionRight());
            pY = std::min(pY, (int)_map->getPixelHeight() - ourUnit.type().dimensionDown());

            Action newAction(action.getID(), action.getPlayerID(), ActionTypes::MOVE, 0, Position(pX, pY));
            ourUnit.move(newAction, getTime());
        }
	}
	else if (action.type() == ActionTypes::RELOAD)
	{
		ourUnit.reload(action, getTime());
	}
	else if (action.type() == ActionTypes::PASS)
	{
		ourUnit.pass(action, getTime());
	}
    else
    {
        SPARCRAFT_ASSERT(false, "Invalid Action Type: %d", (int)action.type());
    }

    SPARCRAFT_ASSERT(ourUnit.firstTimeFree() > gameTime, "Action did not increase unit cooldown, Game Time is: %d\n\n%s", gameTime, ourUnit.debugString().c_str());
}

void GameState::doMove(const Move & moves)
{    
    for (size_t m(0); m<moves.size(); ++m)
    {
        doAction(moves[m]);
    }

    const TimeType nextUnitActTime = std::min(getTimeNextUnitCanAct(0), getTimeNextUnitCanAct(1));
    SPARCRAFT_ASSERT(nextUnitActTime > getTime(), "doMove didn't result in game time advancing");

    updateGameTime(nextUnitActTime);
}

void GameState::doMove(const Move & m1, const Move & m2, bool advanceGameTime)
{
    const TimeType prevUnitActTime = std::min(getTimeNextUnitCanAct(0), getTimeNextUnitCanAct(1));

    for (size_t m(0); m<m1.size(); ++m)
    {
        doAction(m1[m]);
    }

    for (size_t m(0); m<m2.size(); ++m)
    {
        doAction(m2[m]);
    }

    const TimeType nextUnitActTime = std::min(getTimeNextUnitCanAct(0), getTimeNextUnitCanAct(1));
    SPARCRAFT_ASSERT(nextUnitActTime > getTime(), "doMove didn't result in game time advancing: %d %d", m1.size(), m2.size());

    if (advanceGameTime)
    {
        updateGameTime(nextUnitActTime);
    }
}

const std::vector<size_t> & GameState::getUnitIDs(const size_t & player) const
{
    return _unitData.getUnitIDs(player);
}

const Unit & GameState::getUnitByID(const size_t & unitID) const
{
    return _unitData.getUnitByID(unitID);
}

Unit & GameState::_getUnitByID(const size_t & unitID)
{
    return _unitData.getUnitByID(unitID);
}

bool GameState::isWalkable(const Position & pos) const
{
    return _map ? _map->isWalkable(pos) : true;
}

bool GameState::isFlyable(const Position & pos) const
{
    return _map ? _map->isFlyable(pos) : true;
}

size_t GameState::getEnemy(const size_t & player) const
{
	return (player + 1) % 2;
}

// Add a given unit to the state
// This function will give the unit a unique unitID
void GameState::addUnit(const Unit & u)
{
    if (!System::UnitTypeSupported(u.type()))
    {
        std::cerr << "Skipping un-supported unit type: " << u.type().getName() << "\n";
    }

    _unitData.addUnit(u);
}

Unit & GameState::_getUnit(const size_t & player, const size_t & unitIndex)
{
    return _unitData.getUnit(player, unitIndex);
}

const Unit & GameState::getUnit(const size_t & player, const size_t & unitIndex) const
{
    const Unit & unit = _unitData.getUnit(player, unitIndex);

    if (!unit.isAlive())
    {
        std::cout << "Returning a dead unit: Player=" << player << " Index=" << unitIndex << " Unit Type: " << unit.type().getName() << "\n";
    }

    SPARCRAFT_ASSERT(_unitData.getUnit(player, unitIndex).isAlive(), "Trying to get dead unit");

    return _unitData.getUnit(player, unitIndex);
}

bool GameState::playerDead(const size_t & player) const
{
    return numUnits(player) <= 0;
}

size_t GameState::winner() const
{
    if (playerDead(Players::Player_One) && playerDead(Players::Player_Two))
    {
        return Players::Player_None;
    }
    else if (playerDead(Players::Player_One))
    {
        return Players::Player_Two;
    }
    else if (playerDead(Players::Player_Two))
    {
        return Players::Player_One;
    }
    else
    {
        return Players::Player_None;
    }
}

size_t GameState::whoCanMove() const
{
	TimeType p1Time = getTimeNextUnitCanAct(0); //getUnit(0,0).firstTimeFree();
	TimeType p2Time = getTimeNextUnitCanAct(1); //getUnit(1,0).firstTimeFree();

    if (p1Time > getTime() && p2Time > getTime())
    {
        return Players::Player_None;
    }

	if (p1Time < p2Time)
	{
		return Players::Player_One;
	}
	else if (p1Time > p2Time)
	{
		return Players::Player_Two;
	}
	else
	{
		return Players::Player_Both;
	}
}

TimeType GameState::getTimeNextUnitCanAct(const size_t & player) const
{
    int minTime = std::numeric_limits<int>::max();
    for (size_t u(0); u < numUnits(player); ++u)
    {
        minTime = std::min(getUnit(player, u).firstTimeFree(), minTime);
    }

    return minTime;
}

void GameState::updateGameTime(const TimeType nextTime)
{
	const TimeType difference = nextTime - _currentTime;
    _currentTime = nextTime;
	if (difference > 0)
	{
		for (auto& unit : _unitData.getAllUnits())
		{
			const auto unitTypeRequireRemoval = unit.type() == BWAPI::UnitTypes::Zerg_Broodling;
			if (unitTypeRequireRemoval)
			{
				const auto timer = unit.getRemoveTimer();
				const auto newTimer = std::max(0, timer - difference);
				unit.setRemoveTimer(newTimer);
				if (newTimer == 0)
				{
					unit.updateCurrentHP(0);
					_unitData.killUnit(unit.getID());
				}
			}
		}
	}
}

void GameState::setMap(std::shared_ptr<Map> map)
{
	_map = map;

    // check to see if all units are on walkable tiles
    for (size_t p(0); p<Players::Num_Players; ++p)
    {
        for (size_t u(0); u<numUnits(p); ++u)
        {
            const Position & pos(getUnit(p, u).pos());

            //SPARCRAFT_ASSERT(isWalkable(pos), "Unit initial position on non-walkable map tile");
        }
    }
}

size_t GameState::numUnits(const size_t & player) const
{
    return _unitData.numUnits(player);
}

bool GameState::bothCanMove() const
{
	return whoCanMove() == Players::Player_Both;
}

void GameState::setTime(const TimeType & time)
{
	_currentTime = time;
}

size_t GameState::getNumMovements(const size_t & player) const
{
	return _numMovements[player];
}

TimeType GameState::getTime() const
{
	return _currentTime;
}

// detect if there is a deadlock, such that no team can possibly win
bool GameState::gameOver() const
{
    // if someone is dead, then nobody can move
    if (playerDead(Players::Player_One) || playerDead(Players::Player_Two))
    {
        return true;
    }

    // If any unit on any player's side is a mobile attacker, then there is no deadlock
	for (size_t p(0); p<Players::Num_Players; ++p)
	{
		for (size_t u(0); u<numUnits(p); ++u)
		{
			// if any unit on any team is a mobile attacker
			if (getUnit(p, u).isMobile())
			{
				// there is no deadlock, so return false
				return false;
			}
		}
	}

	// at this point we know everyone must be immobile, so check for attack deadlock
    bool attackDeadlock = true;
	for (size_t u1(0); attackDeadlock && u1<numUnits(Players::Player_One); ++u1)
	{
		const Unit & unit1(getUnit(Players::Player_One, u1));
		for (size_t u2(0); attackDeadlock && u2<numUnits(Players::Player_Two); ++u2)
		{
			const Unit & unit2(getUnit(Players::Player_Two, u2));

            // if unit1 is mobile and can target unit2 or vice versa, there is no deadlock
            if (unit1.type().canMove() && unit1.canTarget(unit2) || unit2.type().canMove() && unit2.canTarget(unit1))
            {
                attackDeadlock = false;
                break;
            }
		}
	}

    return attackDeadlock;
}

std::shared_ptr<Map> GameState::getMap() const
{
	return _map;
}

const std::vector<Unit> & GameState::getAllUnits() const
{
    return _unitData.getAllUnits();
}