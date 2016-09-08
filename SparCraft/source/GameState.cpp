#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "SparCraftAssert.h"

using namespace SparCraft;

GameState::GameState()
	: _map(NULL)
	, _currentTime(0)
    , _sameHPFrames(0)
{
    _numMovements[0] = 0;
    _numMovements[1] = 0;
}

void GameState::doAction(const Action & action)
{
	Unit & ourUnit		= _getUnitByID(action.getID());
	size_t player		= ourUnit.getPlayerID();
	size_t enemyPlayer  = getEnemy(player);

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
            int pX = action.pos().x();
            int pY = action.pos().y();

            if (pX < ourUnit.type().dimensionLeft()) 
            { 
                pX = ourUnit.type().dimensionLeft(); 
            }
            
            if (pY < ourUnit.type().dimensionUp())
            { 
                pY = ourUnit.type().dimensionUp();
            }

            if (pX > (int)_map->getPixelWidth() - ourUnit.type().dimensionRight()) 
            { 
                pX = _map->getPixelWidth() - ourUnit.type().dimensionRight(); 
            }

            if (pY > (int)_map->getPixelHeight() - ourUnit.type().dimensionDown())
            { 
                pY = _map->getPixelHeight() - ourUnit.type().dimensionDown(); 
            }

            Action newAction(action.getID(), action.getPlayerID(), ActionTypes::MOVE, 0, Position(pX, pY));

            ourUnit.move(newAction, getTime());
        }
	}
	else if (action.type() == ActionTypes::HEAL)
	{
		Unit & ourOtherUnit = _getUnitByID(action.getTargetID());
			
		// attack the unit
		ourUnit.heal(action, ourOtherUnit, _currentTime);
			
		if (ourOtherUnit.isAlive())
		{
			ourOtherUnit.takeHeal(ourUnit);
		}
	}
	else if (action.type() == ActionTypes::RELOAD)
	{
		ourUnit.waitUntilAttack(action, _currentTime);
	}
	else if (action.type() == ActionTypes::PASS)
	{
		ourUnit.pass(action, _currentTime);
	}
    else
    {
        SPARCRAFT_ASSERT(false, "Invalid Action Type: %d", (int)action.type());
    }
}

void GameState::doMove(const Move & moves)
{    
    for (size_t m(0); m<moves.size(); ++m)
    {
        doAction(moves[m]);
    }

    const TimeType nextUnitActTime = std::min(getTimeNextUnitCanAct(0), getTimeNextUnitCanAct(1));
    SPARCRAFT_ASSERT(nextUnitActTime > getTime(), "doMove didn't result in game time advancing");

    updateGameTime();
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
    SPARCRAFT_ASSERT(nextUnitActTime > getTime(), "doMove didn't result in game time advancing");

    if (advanceGameTime)
    {
        updateGameTime();
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
	if (_map)
	{
		return _map->isWalkable(pos);
	}

	// if there is no map, then return true
	return true;
}

bool GameState::isFlyable(const Position & pos) const
{
	if (_map)
	{
		return _map->isFlyable(pos);
	}

	// if there is no map, then return true
	return true;
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
    SPARCRAFT_ASSERT(_unitData.getUnit(player, unitIndex).isAlive(), "Trying to get dead unit");

    return _unitData.getUnit(player, unitIndex);
}

bool GameState::playerDead(const size_t & player) const
{
	if (numUnits(player) <= 0)
	{
		return true;
	}

	return false;
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

	// if player one is to move first
	if (p1Time < p2Time)
	{
		return Players::Player_One;
	}
	// if player two is to move first
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

void GameState::updateGameTime()
{
    _currentTime = std::min(getTimeNextUnitCanAct(0), getTimeNextUnitCanAct(1));
}

void GameState::setMap(std::shared_ptr<Map> map)
{
	_map = map;

    // check to see if all units are on walkable tiles
    for (size_t p(0); p<Constants::Num_Players; ++p)
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

    if (_sameHPFrames > 200)
    {
        return true;
    }

	for (size_t p(0); p<Constants::Num_Players; ++p)
	{
		for (size_t u(0); u<numUnits(p); ++u)
		{
			// if any unit on any team is a mobile attacker
			if (getUnit(p, u).isMobile() && !getUnit(p, u).canHeal())
			{
				// there is no deadlock, so return false
				return false;
			}
		}
	}

	// at this point we know everyone must be immobile, so check for attack deadlock
	for (size_t u1(0); u1<numUnits(Players::Player_One); ++u1)
	{
		const Unit & unit1(getUnit(Players::Player_One, u1));

		for (size_t u2(0); u2<numUnits(Players::Player_Two); ++u2)
		{
			const Unit & unit2(getUnit(Players::Player_Two, u2));

			// if anyone can attack anyone else
			if (unit1.canTarget(unit2) || unit2.canTarget(unit1))
			{
				// then there is no deadlock
				return false;
			}
		}
	}
	
	// if everyone is immobile and nobody can attack, then there is a deadlock
	return true;
}

std::shared_ptr<Map> GameState::getMap() const
{
	return _map;
}

const std::vector<Unit> & GameState::getAllUnits() const
{
    return _unitData.getAllUnits();
}