#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "SparCraftAssert.h"

using namespace SparCraft;

#define TABS(N) for (int i(0); i<N; ++i) { fprintf(stderr, "\t"); }

// default constructor
GameState::GameState()
	: _map(NULL)
	, _currentTime(0)
	, _maxUnits(Constants::Max_Units)
    , _sameHPFrames(0)
{
	_numMovements.fill(0);
    _prevHPSum.fill(0);
}

// construct state from a save file
GameState::GameState(const std::string & filename)
{
    read(filename);
}

void GameState::performAction(const Action & action)
{
	Unit & ourUnit		= _getUnitByID(action.getID());
	PlayerID player		= ourUnit.getPlayerID();
	PlayerID enemyPlayer  = getEnemy(player);

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

		ourUnit.move(action, getTime());
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

    updateGameTime();
}

void GameState::makeMoves(const std::vector<Action> & moves)
{    
    if (moves.size() > 0)
    {
        const PlayerID canMove = whoCanMove();
        const PlayerID playerToMove = moves[0].getPlayerID();

        SPARCRAFT_ASSERT(canMove != getEnemy(playerToMove), "GameState Error - Called makeMove() for a player that cannot currently move");
    }
    
    for (size_t m(0); m<moves.size(); ++m)
    {
        performAction(moves[m]);
    }
}

const Unit & GameState::getUnitByID(const UnitID & unitID) const
{
    return _unitData.getUnitByID(unitID);
}

Unit & GameState::_getUnitByID(const UnitID & unitID)
{
    return _unitData.getUnitByID(unitID);
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

const PlayerID GameState::getEnemy(const PlayerID & player) const
{
	return (player + 1) % 2;
}

const Unit & GameState::getClosestOurUnit(const PlayerID & player, const UnitID & unitIndex) const
{
	const Unit & myUnit = getUnit(player,unitIndex);

	size_t minDist(1000000);
	PlayerID minUnitInd(0);

	Position currentPos = myUnit.currentPosition(_currentTime);

	for (PlayerID u(0); u < numUnits(player); ++u)
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

const Unit & GameState::getClosestEnemyUnit(const PlayerID & player, const UnitID & unitIndex, bool checkCloaked) const
{
	const PlayerID enemyPlayer = getEnemy(player);
	const Unit & myUnit = getUnit(player,unitIndex);

	PositionType minDist(1000000);
	PlayerID minUnitInd(0);
    PlayerID minUnitID(255);

	Position currentPos = myUnit.currentPosition(_currentTime);

	for (PlayerID u(0); u < numUnits(enemyPlayer); ++u)
	{
        const Unit & enemyUnit(getUnit(enemyPlayer, u));
		if (checkCloaked&& enemyUnit.type().hasPermanentCloak())
		{
			bool invisible = true;
			for (PlayerID detectorIndex(0); detectorIndex < numUnits(player); ++detectorIndex)
			{
				// unit reference
				const Unit & detector = getUnit(player, detectorIndex);
				if (detector.type().isDetector() && detector.canSeeTarget(enemyUnit, _currentTime))
				{
					invisible = false;
					break;
				}
			}
			if (invisible)
			{
				continue;
			}
		}
        PositionType distSq = myUnit.getDistanceSqToUnit(enemyUnit, _currentTime);

		if ((distSq < minDist))// || ((distSq == minDist) && (enemyUnit.getID() < minUnitID)))
		{
			minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.getID();
		}
        else if ((distSq == minDist) && (enemyUnit.getID() < minUnitID))
        {
            minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.getID();
        }
	}

	return getUnit(enemyPlayer, minUnitInd);
}

const bool GameState::checkFull(const PlayerID & player) const
{
    SPARCRAFT_ASSERT(numUnits(player) < Constants::Max_Units, "GameState has too many units");

    return false;
}

// Add a given unit to the state
// This function will give the unit a unique unitID
void GameState::addUnit(const Unit & u)
{
    checkFull(u.getPlayerID());
    System::checkSupportedUnitType(u.type());

    // Calculate the unitID for this unit
    // This will just be the current total number of units in the state
    _unitData.addUnit(u);
}

// Add a unit with given parameters to the state
// This function will give the unit a unique unitID
void GameState::addUnit(const BWAPI::UnitType type, const PlayerID playerID, const Position & pos)
{
    checkFull(playerID);
    System::checkSupportedUnitType(type);

    _unitData.addUnit(Unit(type, playerID, pos));
}

Unit & GameState::_getUnit(const PlayerID & player, const UnitCountType & unitIndex)
{
    return _unitData.getUnit(player, unitIndex);
}

const Unit & GameState::getUnit(const PlayerID & player, const UnitCountType & unitIndex) const
{
    return _unitData.getUnit(player, unitIndex);
}

const size_t GameState::closestEnemyUnitDistance(const Unit & unit) const
{
	PlayerID enemyPlayer(getEnemy(unit.getPlayerID()));

	size_t closestDist(0);

	for (PlayerID u(0); u<numUnits(enemyPlayer); ++u)
	{
        size_t dist(unit.getDistanceSqToUnit(getUnit(enemyPlayer, u), _currentTime));

		if (dist > closestDist)
		{
			closestDist = dist;
		}
	}

	return closestDist;
}

const bool GameState::playerDead(const PlayerID & player) const
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

const PlayerID GameState::whoCanMove() const
{
	TimeType p1Time = getTimeNextUnitCanAct(0); //getUnit(0,0).firstTimeFree();
	TimeType p2Time = getTimeNextUnitCanAct(1); //getUnit(1,0).firstTimeFree();

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
    std::set<PlayerID> unitIDs;

    for (size_t p(0); p<Constants::Num_Players; ++p)
    {
        for (size_t u(0); u<numUnits(p); ++u)
        {
            PlayerID unitID(getUnit(p, u).getID());
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

const TimeType GameState::getTimeNextUnitCanAct(const PlayerID & player) const
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
	const PlayerID who = whoCanMove();

	// if the first player is to move, set the time to his time
	if (who == Players::Player_One)
	{
		_currentTime = getTimeNextUnitCanAct(0); // getUnit(Players::Player_One, 0).firstTimeFree();
	}
	// otherwise it is player two or both, so it's equal to player two's time
	else
	{
		_currentTime = getTimeNextUnitCanAct(1); // getUnit(Players::Player_Two, 0).firstTimeFree();
	}
}

const StateEvalScore GameState::eval(const PlayerID & player, const PlayerID & evalMethod, const PlayerID p1Script, const PlayerID p2Script) const
{
	StateEvalScore score;
	const PlayerID enemyPlayer(getEnemy(player));

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
const ScoreType GameState::evalLTD(const PlayerID & player) const
{
	const PlayerID enemyPlayer(getEnemy(player));
	
	return LTD(player) - LTD(enemyPlayer);
}

// evaluate the state for _playerToMove
const ScoreType GameState::evalLTD2(const PlayerID & player) const
{
	const PlayerID enemyPlayer(getEnemy(player));

	return LTD2(player) - LTD2(enemyPlayer);
}

const StateEvalScore GameState::evalSim(const PlayerID & player, const PlayerID & p1Script, const PlayerID & p2Script) const
{
	const PlayerID p1Model = (p1Script == PlayerModels::Random) ? PlayerModels::NOKDPS : p1Script;
	const PlayerID p2Model = (p2Script == PlayerModels::Random) ? PlayerModels::NOKDPS : p2Script;

	PlayerPtr p1(AllPlayers::getPlayerPtr(Players::Player_One, p1Model));
	PlayerPtr p2(AllPlayers::getPlayerPtr(Players::Player_Two, p2Model));

	Game game(*this, p1, p2, 200);

	game.play();

	ScoreType evalReturn = game.getState().evalLTD2(player);

	return StateEvalScore(evalReturn, game.getState().getNumMovements(player));
}

void GameState::calculateStartingHealth()
{
	for (PlayerID p(0); p<Constants::Num_Players; ++p)
	{
		float totalHP(0);
		float totalSQRT(0);

		for (PlayerID u(0); u < numUnits(p); ++u)
		{
			totalHP += getUnit(p, u).maxHP() * getUnit(p, u).dpf();
			totalSQRT += sqrtf(getUnit(p,u).maxHP()) * getUnit(p, u).dpf();;
		}

		_totalLTD[p] = totalHP;
		_totalSumSQRT[p] = totalSQRT;
	}
}

const ScoreType	GameState::LTD2(const PlayerID & player) const
{
	if (numUnits(player) == 0)
	{
		return 0;
	}

	float sum(0);

	for (PlayerID u(0); u < numUnits(player); ++u)
	{
		const Unit & unit(getUnit(player, u));

		sum += sqrtf(unit.currentHP()) * unit.dpf();
	}

	ScoreType ret = (ScoreType)(1000 * sum / _totalSumSQRT[player]);

	return ret;
}

const ScoreType GameState::LTD(const PlayerID & player) const
{
	if (numUnits(player) == 0)
	{
		return 0;
	}

	float sum(0);

	for (PlayerID u(0); u<numUnits(player); ++u)
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

            SPARCRAFT_ASSERT(isWalkable(pos), "Unit initial position on non-walkable map tile");
        }
    }
}

const size_t GameState::numUnits(const PlayerID & player) const
{
    return _unitData.numUnits(player);
}

const bool GameState::bothCanMove() const
{
	return getUnit(0, 0).firstTimeFree() == getUnit(1, 0).firstTimeFree();
}

void GameState::setTime(const TimeType & time)
{
	_currentTime = time;
}

const int & GameState::getNumMovements(const PlayerID & player) const
{
	return _numMovements[player];
}

const TimeType GameState::getTime() const
{
	return _currentTime;
}

const float & GameState::getTotalLTD(const PlayerID & player) const
{
	return _totalLTD[player];
}

const float & GameState::getTotalLTD2(const PlayerID & player)	const
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

// print the state in a neat way
void GameState::print(int indent) const
{
	TABS(indent);
	std::cout << calculateHash(0) << "\n";
	fprintf(stderr, "State - Time: %d\n", _currentTime);

	for (PlayerID p(0); p<Constants::Num_Players; ++p)
	{
		for (UnitCountType u(0); u < numUnits(p); ++u)
		{
			const Unit & unit(getUnit(p, u));

			TABS(indent);
			fprintf(stderr, "  P%d %5d %5d    (%3d, %3d)     %s\n", unit.getPlayerID(), unit.currentHP(), unit.firstTimeFree(), unit.x(), unit.y(), unit.name().c_str());
		}
	}
	fprintf(stderr, "\n\n");
}

std::string GameState::toString() const
{

	std::stringstream ss;

	ss << calculateHash(0) << "\n";
	ss << "Time: " << _currentTime << std::endl;

	for (PlayerID p(0); p<Constants::Num_Players; ++p)
	{
		for (UnitCountType u(0); u < numUnits(p); ++u)
		{
			const Unit & unit(getUnit(p, u));

			ss << "  P" << (int)unit.getPlayerID() << " " << unit.currentHP() << " (" << unit.x() << ", " << unit.y() << ") " << unit.name() << std::endl;
		}
	}
	ss << std::endl;

	return ss.str();
}

std::string GameState::toStringCompact() const
{
	std::stringstream ss;

	for (PlayerID p(0); p<Constants::Num_Players; ++p)
	{
        std::map<BWAPI::UnitType, size_t> typeCount;

		for (UnitCountType u(0); u < numUnits(p); ++u)
		{
			const Unit & unit(getUnit(p, u));

            if (typeCount.find(unit.type()) != std::end(typeCount))
            {
                typeCount[unit.type()]++;
            }
            else
            {
                typeCount[unit.type()] = 1;
            }
		}

        for (auto & kv : typeCount)
        {
            const BWAPI::UnitType & type = kv.first;
            const size_t count = kv.second;

            ss << "P" << (int)p << " " << count << " " << type.getName() << "\n";
        }
	}

	return ss.str();
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


const HashType GameState::calculateHash(const size_t & hashNum) const
{
	HashType hash(0);

	for (PlayerID p(0); p < Constants::Num_Players; ++p)
	{
		for (PlayerID u(0); u < numUnits(p); ++u)
		{
			hash ^= Hash::magicHash(getUnit(p,u).calculateHash(hashNum, _currentTime), p, u);
		}
	}

	return hash;
}