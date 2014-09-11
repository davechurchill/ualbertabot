#include "GameState.h"
#include "Player.h"
#include "Game.h"

using namespace SparCraft;

#define TABS(N) for (int i(0); i<N; ++i) { fprintf(stderr, "\t"); }

class UnitIndexCompare
{
    const GameState & state;
    int player;

public:

    UnitIndexCompare(const GameState & s, const int & p)
        : state(s)
        , player(p)
    {

    }

	const bool operator() (const int & u1, const int & u2) const
	{
        return state.getUnitDirect(player, u1) < state.getUnitDirect(player, u2);
    }
};

// default constructor
GameState::GameState()
	: _map(NULL)
	, _currentTime(0)
	, _maxUnits(Constants::Max_Units)
    , _sameHPFrames(0)
{
	_numUnits.fill(0);
	_prevNumUnits.fill(0);
	_numMovements.fill(0);
    _prevHPSum.fill(0);

	for (size_t u(0); u<_maxUnits; ++u)
	{
        _unitIndex[0][u] = u;
		_unitIndex[1][u] = u;
	}
}

// construct state from a save file
GameState::GameState(const std::string & filename)
{
    read(filename);
}

// call this whenever we are done with moves
void GameState::finishedMoving()
{
	// sort the unit vector based on time left to move
	sortUnits();

	// update the current time of the state
	updateGameTime();

    // calculate the hp sum of each player
    int hpSum[2];
    for (IDType p(0); p<Constants::Num_Players; ++p)
	{
		hpSum[p] = 0;

		for (IDType u(0); u<numUnits(p); ++u)
		{ 
            hpSum[p] += getUnit(p, u).currentHP();
        }
    }

    // if the hp sums match the last hp sum
    if (hpSum[0] == _prevHPSum[0] && hpSum[1] == _prevHPSum[1])
    {
        _sameHPFrames++;
    }
    else
    {
        _sameHPFrames = 0;
    }

    for (IDType p(0); p<Constants::Num_Players; ++p)
	{
        _prevHPSum[p] = hpSum[p];
    }
}

const HashType GameState::calculateHash(const size_t & hashNum) const
{
	HashType hash(0);

	for (IDType p(0); p < Constants::Num_Players; ++p)
	{
		for (IDType u(0); u < _numUnits[p]; ++u)
		{
			hash ^= Hash::magicHash(getUnit(p,u).calculateHash(hashNum, _currentTime), p, u);
		}
	}

	return hash;
}

void GameState::generateMoves(MoveArray & moves, const IDType & playerIndex) const
{
	moves.clear();

    // which is the enemy player
	IDType enemyPlayer  = getEnemy(playerIndex);

    // make sure this player can move right now
    const IDType canMove(whoCanMove());
    if (canMove == enemyPlayer)
    {
        System::FatalError("GameState Error - Called generateMoves() for a player that cannot currently move");
    }

	// we are interested in all simultaneous moves
	// so return all units which can move at the same time as the first
	TimeType firstUnitMoveTime = getUnit(playerIndex, 0).firstTimeFree();
		
	for (IDType unitIndex(0); unitIndex < _numUnits[playerIndex]; ++unitIndex)
	{
		// unit reference
		const Unit & unit(getUnit(playerIndex,unitIndex));
			
		// if this unit can't move at the same time as the first
		if (unit.firstTimeFree() != firstUnitMoveTime)
		{
			// stop checking
			break;
		}

		if (unit.previousActionTime() == _currentTime && _currentTime != 0)
		{
            System::FatalError("Previous Move Took 0 Time: " + unit.previousAction().moveString());
		}

		moves.addUnit();

		// generate attack moves
		if (unit.canAttackNow())
		{
			for (IDType u(0); u<_numUnits[enemyPlayer]; ++u)
			{
				const Unit & enemyUnit(getUnit(enemyPlayer, u));
				if (unit.canAttackTarget(enemyUnit, _currentTime) && enemyUnit.isAlive())
				{
					moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::ATTACK, u));
                    //moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::ATTACK, unit.ID()));
				}
			}
		}
		else if (unit.canHealNow())
		{
			for (IDType u(0); u<_numUnits[playerIndex]; ++u)
			{
				// units cannot heal themselves in broodwar
				if (u == unitIndex)
				{
					continue;
				}

				const Unit & ourUnit(getUnit(playerIndex, u));
				if (unit.canHealTarget(ourUnit, _currentTime) && ourUnit.isAlive())
				{
					moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::HEAL, u));
                    //moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::HEAL, unit.ID()));
				}
			}
		}
		// generate the wait move if it can't attack yet
		else
		{
			if (!unit.canHeal())
			{
				moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::RELOAD, 0));
			}
		}
		
		// generate movement moves
		if (unit.isMobile())
		{
            // In order to not move when we could be shooting, we want to move for the minimum of:
            // 1) default move distance move time
            // 2) time until unit can attack, or if it can attack, the next cooldown
            double timeUntilAttack          = unit.nextAttackActionTime() - getTime();
            timeUntilAttack                 = timeUntilAttack == 0 ? unit.attackCooldown() : timeUntilAttack;

            // the default move duration
            double defaultMoveDuration      = (double)Constants::Move_Distance / unit.speed();

            // if we can currently attack
            double chosenTime               = std::min(timeUntilAttack, defaultMoveDuration);

            // the chosen movement distance
            PositionType moveDistance       = (PositionType)(chosenTime * unit.speed());

            // DEBUG: If chosen move distance is ever 0, something is wrong
            if (moveDistance == 0)
            {
                System::FatalError("Move Action with distance 0 generated");
            }

            // we are only generating moves in the cardinal direction specified in common.h
			for (IDType d(0); d<Constants::Num_Directions; ++d)
			{			
                // the direction of this movement
              	Position dir(Constants::Move_Dir[d][0], Constants::Move_Dir[d][1]);
            
                if (moveDistance == 0)
                {
                    printf("%lf %lf %lf\n", timeUntilAttack, defaultMoveDuration, chosenTime);
                }

                // the final destination position of the unit
                Position dest = unit.pos() + Position(moveDistance*dir.x(), moveDistance*dir.y());

                // if that poisition on the map is walkable
                if (isWalkable(dest) || (unit.type().isFlyer() && isFlyable(dest)))
				{
                    // add the move to the MoveArray
					moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::MOVE, d, dest));
				}
			}
		}

		// if no moves were generated for this unit, it must be issued a 'PASS' move
		if (moves.numMoves(unitIndex) == 0)
		{
			moves.add(UnitAction(unitIndex, playerIndex, UnitActionTypes::PASS, 0));
		}
	}
}


void GameState::makeMoves(const std::vector<UnitAction> & moves)
{    
    if (moves.size() > 0)
    {
        const IDType canMove(whoCanMove());
        const IDType playerToMove(moves[0].player());
        if (canMove == getEnemy(playerToMove))
        {
            System::FatalError("GameState Error - Called makeMove() for a player that cannot currently move");
        }
    }
    
    for (size_t m(0); m<moves.size(); ++m)
    {
        performUnitAction(moves[m]);
    }
}

void GameState::performUnitAction(const UnitAction & move)
{
	Unit & ourUnit		= getUnit(move._player, move._unit);
	IDType player		= ourUnit.player();
	IDType enemyPlayer  = getEnemy(player);

	if (move._moveType == UnitActionTypes::ATTACK)
	{
		Unit & enemyUnit(getUnit(enemyPlayer,move._moveIndex));
        //Unit & enemyUnit(getUnitByID(enemyPlayer ,move._moveIndex));
			
		// attack the unit
		ourUnit.attack(move, enemyUnit, _currentTime);
			
		// enemy unit takes damage if it is alive
		if (enemyUnit.isAlive())
		{				
			enemyUnit.takeAttack(ourUnit);

			// check to see if enemy unit died
			if (!enemyUnit.isAlive())
			{
				// if it died, remove it
				_numUnits[enemyPlayer]--;
			}
		}			
	}
	else if (move._moveType == UnitActionTypes::MOVE)
	{
		_numMovements[player]++;

		ourUnit.move(move, _currentTime);
	}
	else if (move._moveType == UnitActionTypes::HEAL)
	{
		Unit & ourOtherUnit(getUnit(player,move._moveIndex));
			
		// attack the unit
		ourUnit.heal(move, ourOtherUnit, _currentTime);
			
		if (ourOtherUnit.isAlive())
		{
			ourOtherUnit.takeHeal(ourUnit);
		}
	}
	else if (move._moveType == UnitActionTypes::RELOAD)
	{
		ourUnit.waitUntilAttack(move, _currentTime);
	}
	else if (move._moveType == UnitActionTypes::PASS)
	{
		ourUnit.pass(move, _currentTime);
	}
}

const Unit & GameState::getUnitByID(const IDType & unitID) const
{
	for (IDType p(0); p<Constants::Num_Players; ++p)
	{
		for (IDType u(0); u<numUnits(p); ++u)
		{
			if (getUnit(p, u).ID() == unitID)
			{
				return getUnit(p, u);
			}
		}
	}

    System::FatalError("GameState Error: getUnitByID() Unit not found");
	return getUnit(0,0);
}

const Unit & GameState::getUnitByID(const IDType & player, const IDType & unitID) const
{
	for (IDType u(0); u<numUnits(player); ++u)
	{
		if (getUnit(player, u).ID() == unitID)
		{
			return getUnit(player, u);
		}
	}

	System::FatalError("GameState Error: getUnitByID() Unit not found");
	return getUnit(0,0);
}

Unit & GameState::getUnitByID(const IDType & player, const IDType & unitID) 
{
	for (IDType u(0); u<numUnits(player); ++u)
	{
		if (getUnit(player, u).ID() == unitID)
		{
			return getUnit(player, u);
		}
	}

	System::FatalError("GameState Error: getUnitByID() Unit not found");
	return getUnit(0,0);
}

const bool GameState::isWalkable(const Position & pos) const
{
	if (_map)
	{
		return _map->isWalkable(pos);
	}

	// if there is no map, then return true
	return true;
}

const bool GameState::isFlyable(const Position & pos) const
{
	if (_map)
	{
		return _map->isFlyable(pos);
	}

	// if there is no map, then return true
	return true;
}

const IDType GameState::getEnemy(const IDType & player) const
{
	return (player + 1) % 2;
}

const Unit & GameState::getClosestOurUnit(const IDType & player, const IDType & unitIndex)
{
	const Unit & myUnit(getUnit(player,unitIndex));

	size_t minDist(1000000);
	IDType minUnitInd(0);

	Position currentPos = myUnit.currentPosition(_currentTime);

	for (IDType u(0); u<_numUnits[player]; ++u)
	{
		if (u == unitIndex || getUnit(player, u).canHeal())
		{
			continue;
		}

		//size_t distSq(myUnit.distSq(getUnit(enemyPlayer,u)));
		size_t distSq(currentPos.getDistanceSq(getUnit(player, u).currentPosition(_currentTime)));

		if (distSq < minDist)
		{
			minDist = distSq;
			minUnitInd = u;
		}
	}

	return getUnit(player, minUnitInd);
}

const Unit & GameState::getClosestEnemyUnit(const IDType & player, const IDType & unitIndex)
{
	const IDType enemyPlayer(getEnemy(player));
	const Unit & myUnit(getUnit(player,unitIndex));

	PositionType minDist(1000000);
	IDType minUnitInd(0);
    IDType minUnitID(255);

	Position currentPos = myUnit.currentPosition(_currentTime);

	for (IDType u(0); u<_numUnits[enemyPlayer]; ++u)
	{
        Unit & enemyUnit(getUnit(enemyPlayer, u));
        PositionType distSq = myUnit.getDistanceSqToUnit(enemyUnit, _currentTime);

		if ((distSq < minDist))// || ((distSq == minDist) && (enemyUnit.ID() < minUnitID)))
		{
			minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.ID();
		}
        else if ((distSq == minDist) && (enemyUnit.ID() < minUnitID))
        {
            minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.ID();
        }
	}

	return getUnit(enemyPlayer, minUnitInd);
}

const bool GameState::checkFull(const IDType & player) const
{
    if (numUnits(player) >= Constants::Max_Units)
    {
        std::stringstream ss;
        ss << "GameState has too many units. Constants::Max_Units = " << Constants::Max_Units;
        System::FatalError(ss.str());
        return false;
    }

    return false;
}

// Add a given unit to the state
// This function will give the unit a unique unitID
void GameState::addUnit(const Unit & u)
{
    checkFull(u.player());
    System::checkSupportedUnitType(u.type());

    // Calculate the unitID for this unit
    // This will just be the current total number of units in the state
    IDType unitID = _numUnits[Players::Player_One] + _numUnits[Players::Player_Two];

    // Set the unit and it's unitID
	getUnit(u.player(), _numUnits[u.player()]) = u;
    getUnit(u.player(), _numUnits[u.player()]).setUnitID(unitID);

    // Increment the number of units this player has
	_numUnits[u.player()]++;
	_prevNumUnits[u.player()]++;

    // And do the clean-up
	finishedMoving();
	calculateStartingHealth();

    if (!checkUniqueUnitIDs())
    {
        System::FatalError("GameState has non-unique Unit ID values");
    }
}

// Add a unit with given parameters to the state
// This function will give the unit a unique unitID
void GameState::addUnit(const BWAPI::UnitType type, const IDType playerID, const Position & pos)
{
    checkFull(playerID);
    System::checkSupportedUnitType(type);

    // Calculate the unitID for this unit
    // This will just be the current total number of units in the state
    IDType unitID = _numUnits[Players::Player_One] + _numUnits[Players::Player_Two];

    // Set the unit and it's unitID
	getUnit(playerID, _numUnits[playerID]) = Unit(type, playerID, pos);
    getUnit(playerID, _numUnits[playerID]).setUnitID(unitID);

    // Increment the number of units this player has
	_numUnits[playerID]++;
	_prevNumUnits[playerID]++;

    // And do the clean-up
	finishedMoving();
	calculateStartingHealth();

    if (!checkUniqueUnitIDs())
    {
        System::FatalError("GameState has non-unique Unit ID values");
    }
}

// Add a given unit to the state
// This function will keep the unit ID assigned by player. Only use this for advanced / BWAPI states
void GameState::addUnitWithID(const Unit & u)
{
    checkFull(u.player());
    System::checkSupportedUnitType(u.type());

    // Simply add the unit to the array
	getUnit(u.player(), _numUnits[u.player()]) = u;

    // Increment the number of units this player has
	_numUnits[u.player()]++;
	_prevNumUnits[u.player()]++;

    // And do the clean-up
	finishedMoving();
	calculateStartingHealth();

    if (!checkUniqueUnitIDs())
    {
        System::FatalError("GameState has non-unique Unit ID values");
    }
}

void GameState::sortUnits()
{
	// sort the units based on time free
	for (size_t p(0); p<Constants::Num_Players; ++p)
	{
		if (_prevNumUnits[p] <= 1)
		{
			_prevNumUnits[p] = _numUnits[p];
			continue;
		}
		else
		{
			/*for (int i=1; i<_prevNumUnits[p]; ++i)
			{
				// A[ i ] is added in the sorted sequence A[0, .. i-1]
				// save A[i] to make a hole at index iHole
				//Unit * item = _unitPtrs[p][i];
                int itemIndex = _unitIndex[p][i];
                Unit & itemUnit = getUnit(p, i);
                int iHole = i;
				// keep moving the hole to next smaller index until A[iHole - 1] is <= item
				//while ((iHole > 0) && (*item < *(_unitPtrs[p][iHole - 1])))
                while ((iHole > 0) && (itemUnit < getUnit(p, iHole-1)))
				{
					// move hole to next smaller index
					//_unitPtrs[p][iHole] = _unitPtrs[p][iHole - 1];
                    _unitIndex[p][iHole] = _unitIndex[p][iHole - 1];
					iHole = iHole - 1;
				}
				// put item in the hole
                _unitIndex[p][iHole] = itemIndex;
				//_unitPtrs[p][iHole] = item;
			}*/
	
			
			//_unitPtrs[p].sort(_prevNumUnits[p], UnitPtrCompare());
            std::sort(&_unitIndex[p][0], &_unitIndex[p][0] + _prevNumUnits[p], UnitIndexCompare(*this, p));
			_prevNumUnits[p] = _numUnits[p];
		}
	}	
}

Unit & GameState::getUnit(const IDType & player, const UnitCountType & unitIndex)
{
    return _units[player][_unitIndex[player][unitIndex]];
}

const Unit & GameState::getUnit(const IDType & player, const UnitCountType & unitIndex) const
{
    return _units[player][_unitIndex[player][unitIndex]];
}

const size_t GameState::closestEnemyUnitDistance(const Unit & unit) const
{
	IDType enemyPlayer(getEnemy(unit.player()));

	size_t closestDist(0);

	for (IDType u(0); u<numUnits(enemyPlayer); ++u)
	{
        size_t dist(unit.getDistanceSqToUnit(getUnit(enemyPlayer, u), _currentTime));

		if (dist > closestDist)
		{
			closestDist = dist;
		}
	}

	return closestDist;
}

const bool GameState::playerDead(const IDType & player) const
{
	if (numUnits(player) <= 0)
	{
		return true;
	}

	for (size_t u(0); u<numUnits(player); ++u)
	{
		if (getUnit(player, u).damage() > 0)
		{
			return false;
		}
	}

	return true;
}

const IDType GameState::whoCanMove() const
{
	TimeType p1Time(getUnit(0,0).firstTimeFree());
	TimeType p2Time(getUnit(1,0).firstTimeFree());

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

const bool GameState::checkUniqueUnitIDs() const
{
    std::set<IDType> unitIDs;

    for (size_t p(0); p<Constants::Num_Players; ++p)
    {
        for (size_t u(0); u<numUnits(p); ++u)
        {
            IDType unitID(getUnit(p, u).ID());
            if (unitIDs.find(unitID) != unitIDs.end())
            {
                return false;
            }
            else
            {
                unitIDs.insert(unitID);
            }
        }
    }

    return true;
}

void GameState::updateGameTime()
{
	const IDType who(whoCanMove());

	// if the first player is to move, set the time to his time
	if (who == Players::Player_One)
	{
		_currentTime = getUnit(Players::Player_One, 0).firstTimeFree();
	}
	// otherwise it is player two or both, so it's equal to player two's time
	else
	{
		_currentTime = getUnit(Players::Player_Two, 0).firstTimeFree();
	}
}

const StateEvalScore GameState::eval(const IDType & player, const IDType & evalMethod, const IDType p1Script, const IDType p2Script) const
{
	StateEvalScore score;
	const IDType enemyPlayer(getEnemy(player));

	// if both players are dead, return 0
	if (playerDead(enemyPlayer) && playerDead(player))
	{
		return StateEvalScore(0, 0);
	}

	StateEvalScore simEval;

	if (evalMethod == SparCraft::EvaluationMethods::LTD)
	{
		score = StateEvalScore(evalLTD(player), 0);
	}
	else if (evalMethod == SparCraft::EvaluationMethods::LTD2)
	{
		score = StateEvalScore(evalLTD2(player), 0);
	}
	else if (evalMethod == SparCraft::EvaluationMethods::Playout)
	{
		score = evalSim(player, p1Script, p2Script);
	}

	if (score.val() == 0)
	{
		return score;
	}

	ScoreType winBonus(0);

	if (playerDead(enemyPlayer) && !playerDead(player))
	{
		winBonus = 100000;
	}
	else if (playerDead(player) && !playerDead(enemyPlayer))
	{
		winBonus = -100000;
	}

	return StateEvalScore(score.val() + winBonus, score.numMoves());
}

// evaluate the state for _playerToMove
const ScoreType GameState::evalLTD(const IDType & player) const
{
	const IDType enemyPlayer(getEnemy(player));
	
	return LTD(player) - LTD(enemyPlayer);
}

// evaluate the state for _playerToMove
const ScoreType GameState::evalLTD2(const IDType & player) const
{
	const IDType enemyPlayer(getEnemy(player));

	return LTD2(player) - LTD2(enemyPlayer);
}

const StateEvalScore GameState::evalSim(const IDType & player, const IDType & p1Script, const IDType & p2Script) const
{
	const IDType p1Model = (p1Script == PlayerModels::Random) ? PlayerModels::NOKDPS : p1Script;
	const IDType p2Model = (p2Script == PlayerModels::Random) ? PlayerModels::NOKDPS : p2Script;

	PlayerPtr p1(AllPlayers::getPlayerPtr(Players::Player_One, p1Model));
	PlayerPtr p2(AllPlayers::getPlayerPtr(Players::Player_Two, p2Model));

	Game game(*this, p1, p2, 200);

	game.play();

	ScoreType evalReturn = game.getState().evalLTD2(player);

	return StateEvalScore(evalReturn, game.getState().getNumMovements(player));
}

void GameState::calculateStartingHealth()
{
	for (IDType p(0); p<Constants::Num_Players; ++p)
	{
		float totalHP(0);
		float totalSQRT(0);

		for (IDType u(0); u<_numUnits[p]; ++u)
		{
			totalHP += getUnit(p, u).maxHP() * getUnit(p, u).dpf();
			totalSQRT += sqrtf(getUnit(p,u).maxHP()) * getUnit(p, u).dpf();;
		}

		_totalLTD[p] = totalHP;
		_totalSumSQRT[p] = totalSQRT;
	}
}

const ScoreType	GameState::LTD2(const IDType & player) const
{
	if (numUnits(player) == 0)
	{
		return 0;
	}

	float sum(0);

	for (IDType u(0); u<numUnits(player); ++u)
	{
		const Unit & unit(getUnit(player, u));

		sum += sqrtf(unit.currentHP()) * unit.dpf();
	}

	ScoreType ret = (ScoreType)(1000 * sum / _totalSumSQRT[player]);

	return ret;
}

const ScoreType GameState::LTD(const IDType & player) const
{
	if (numUnits(player) == 0)
	{
		return 0;
	}

	float sum(0);

	for (IDType u(0); u<numUnits(player); ++u)
	{
		const Unit & unit(getUnit(player, u));

		sum += unit.currentHP() * unit.dpf();
	}

	return (ScoreType)(1000 * sum / _totalLTD[player]);
}

void GameState::setMap(Map * map)
{
	_map = map;

    // check to see if all units are on walkable tiles
    for (size_t p(0); p<Constants::Num_Players; ++p)
    {
        for (size_t u(0); u<numUnits(p); ++u)
        {
            const Position & pos(getUnit(p, u).pos());

            if (!isWalkable(pos))
            {
                std::stringstream ss;
                ss << "Unit initial position on non-walkable map tile: " << getUnit(p, u).name() << " (" << pos.x() << "," << pos.y() << ")";
                System::FatalError(ss.str());
            }
        }
    }
}

const size_t GameState::numUnits(const IDType & player) const
{
	return _numUnits[player];
}

const size_t GameState::prevNumUnits(const IDType & player) const
{
	return _prevNumUnits[player];
}

const Unit & GameState::getUnitDirect(const IDType & player, const IDType & unit) const
{
	return _units[player][unit];
}

const bool GameState::bothCanMove() const
{
	return getUnit(0, 0).firstTimeFree() == getUnit(1, 0).firstTimeFree();
}

void GameState::setTime(const TimeType & time)
{
	_currentTime = time;
}

const int & GameState::getNumMovements(const IDType & player) const
{
	return _numMovements[player];
}

const TimeType GameState::getTime() const
{
	return _currentTime;
}

const float & GameState::getTotalLTD(const IDType & player) const
{
	return _totalLTD[player];
}

const float & GameState::getTotalLTD2(const IDType & player)	const
{
	return _totalSumSQRT[player];
}

void GameState::setTotalLTD(const float & p1, const float & p2)
{
	_totalLTD[Players::Player_One] = p1;
	_totalLTD[Players::Player_Two] = p2;
}

// detect if there is a deadlock, such that no team can possibly win
const bool GameState::isTerminal() const
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
			if (unit1.canAttackTarget(unit2, _currentTime) || unit2.canAttackTarget(unit1, _currentTime))
			{
				// then there is no deadlock
				return false;
			}
		}
	}
	
	// if everyone is immobile and nobody can attack, then there is a deadlock
	return true;
}

void GameState::setTotalLTD2(const float & p1, const float & p2)
{
	_totalSumSQRT[Players::Player_One] = p1;
	_totalSumSQRT[Players::Player_Two] = p2;
}

Map * GameState::getMap() const
{
	return _map;
}

const size_t GameState::numNeutralUnits() const
{
	return _neutralUnits.size();
}

const Unit & GameState::getNeutralUnit(const size_t & u) const
{
	return _neutralUnits[u];
}

void GameState::addNeutralUnit(const Unit & unit)
{
	_neutralUnits.add(unit);
}

// print the state in a neat way
void GameState::print(int indent) const
{
	TABS(indent);
	std::cout << calculateHash(0) << "\n";
	fprintf(stderr, "State - Time: %d\n", _currentTime);

	for (IDType p(0); p<Constants::Num_Players; ++p)
	{
		for (UnitCountType u(0); u<_numUnits[p]; ++u)
		{
			const Unit & unit(getUnit(p, u));

			TABS(indent);
			fprintf(stderr, "  P%d %5d %5d    (%3d, %3d)     %s\n", unit.player(), unit.currentHP(), unit.firstTimeFree(), unit.x(), unit.y(), unit.name().c_str());
		}
	}
	fprintf(stderr, "\n\n");
}

void GameState::write(const std::string & filename) const
{
    std::ofstream fout (filename.c_str(), std::ios::out | std::ios::binary); 
    fout.write((char *)this, sizeof(*this)); 
    fout.close();
}

void GameState::read(const std::string & filename)
{
    std::ifstream fin (filename.c_str(), std::ios::in | std::ios::binary);
    fin.read((char *)this, sizeof(*this));
    fin.close();
}
