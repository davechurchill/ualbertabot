#include "MoveArray.h"

using namespace SparCraft;

MoveArray::MoveArray(const size_t maxUnits) 
	: _numUnits(0)
	, _maxUnits(Constants::Max_Units)
    , _hasMoreMoves(true)
{
    //_currentMovesVec.reserve(Constants::Max_Units);
	_numMoves.fill(0);
    _currentMovesIndex.fill(0);
}
    
void MoveArray::clear() 
{
    // only clear things if they need to be cleared
    if (_numUnits == 0)
    {
        return;
    }

	_numUnits = 0;
	_numMoves.fill(0);
    resetMoveIterator();
}

// shuffle the MOVE unit actions to prevent bias in experiments
// this function assumes that all MOVE actions are contiguous in the moves array
// this should be the case unless you change the move generation ordering
void MoveArray::shuffleMoveActions()
{
    // for each unit
    for (size_t u(0); u<numUnits(); ++u)
    {
        int moveEnd(-1);
        int moveBegin(-1);

        // reverse through the list of actions for this unit
        for (int a(numMoves(u)-1); a >= 0; --a)
        {
            IDType moveType(getMove(u, a).type());

            // mark the end of the move actions
            if (moveEnd == -1 && (moveType == UnitActionTypes::MOVE))
            {
                moveEnd = a;
            }
            // mark the beginning of the MOVE unit actions
            else if ((moveEnd != -1) && (moveBegin == -1) && (moveType != UnitActionTypes::MOVE))
            {
                moveBegin = a;
            }
            else if (moveBegin != -1)
            {
                break;
            }
        }

        // if we found the end but didn't find the beginning
        if (moveEnd != -1 && moveBegin == -1)
        {
            // then the move actions begin at the beginning of the array
            moveBegin = 0;
        }

        // shuffle the movement actions for this unit
        if (moveEnd != -1 && moveBegin != -1 && moveEnd != moveBegin)
        {
            std::random_shuffle(&_moves[u][moveBegin], &_moves[u][moveEnd]);
            resetMoveIterator();
        }
    }
}

// returns a given move from a unit
const UnitAction & MoveArray::getMove(const size_t & unit, const size_t & move) const
{
    assert(_moves[unit][(size_t)move].unit() != 255);

    return _moves[unit][(size_t)move];
}

void MoveArray::printCurrentMoveIndex()
{
    for (size_t u(0); u<_numUnits; ++u)
    {
        printf("%d ", _currentMovesIndex[u]);
    }

    printf("\n");
}

void MoveArray::incrementMove(const size_t & unit)
{
    // increment the index for this unit
    _currentMovesIndex[unit] = (_currentMovesIndex[unit] + 1) % _numMoves[unit];

    // if the value rolled over, we need to do the carry calculation
    if (_currentMovesIndex[unit] == 0)
    {
        // the next unit index
        size_t nextUnit = unit + 1;

        // if we have space left to increment, do it
        if (nextUnit < _numUnits)
        {
            incrementMove(nextUnit);
        }
        // otherwise we have no more moves
        else
        {
            // stop
            _hasMoreMoves = false;
        }
    }

    _currentMoves[unit] = _moves[unit][_currentMovesIndex[unit]];
    //_currentMovesVec[unit] = _moves[unit][_currentMovesIndex[unit]];
}

const bool MoveArray::hasMoreMoves() const
{
    return _hasMoreMoves;
}

void MoveArray::resetMoveIterator()
{
    _hasMoreMoves = true;
    _currentMovesIndex.fill(0);

    for (size_t u(0); u<numUnits(); ++u)
    {
        _currentMoves[u] = _moves[u][_currentMovesIndex[u]];
        //_currentMovesVec[u] = _moves[u][_currentMovesIndex[u]];
    }
}

void MoveArray::getNextMoveVec(std::vector<UnitAction> & moves)
{
    moves.assign(&_currentMoves[0], &_currentMoves[_numUnits]);
    //moves = _currentMovesVec;
    incrementMove(0);
}

const size_t MoveArray::maxUnits() const
{
	return _moves.getRows();
}

// adds a Move to the unit specified
void MoveArray::add(const UnitAction & move)
{
	_moves[move._unit][_numMoves[move._unit]] = move;
	_numMoves[move._unit]++;

    _currentMovesIndex[_numUnits-1] = 0;
    _currentMoves[_numUnits-1] = _moves[move._unit][0];
    //_currentMovesVec.push_back(_moves[move._unit][0]);
    //resetMoveIterator();
}
	
bool MoveArray::validateMoves()
{
	for (size_t u(0); u<numUnits(); ++u)
	{
		for (size_t m(0); m<numMoves(u); ++m)
		{
			const UnitAction & move(getMove(u, m));

			if (move.unit() > 200)
			{
				printf("Unit Move Incorrect! Something will be wrong\n");
				return false;
			}
		}
	}
		
	return true;
}

const IDType MoveArray::getUnitID(const IDType & unit) const
{
	return getMove(unit, 0).unit();
}

const IDType MoveArray::getPlayerID(const IDType & unit) const
{
	return getMove(unit, 0).player();
}

void MoveArray::addUnit() 											{ _numUnits++; }

const size_t & MoveArray::numUnits()						const	{ return _numUnits; }
const size_t & MoveArray::numUnitsInTuple()				const	{ return numUnits(); }
const size_t & MoveArray::numMoves(const size_t & unit)	const	{ return _numMoves[unit]; }