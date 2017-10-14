#include "MoveArray.h"
#include "SparCraftAssert.h"

using namespace SparCraft;

MoveArray::MoveArray() 
	: _hasMoreMoves(true)
{

}
    
void MoveArray::clear() 
{
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
            size_t moveType(getMove(u, a).type());

            // mark the end of the move actions
            if (moveEnd == -1 && (moveType == ActionTypes::MOVE))
            {
                moveEnd = a;
            }
            // mark the beginning of the MOVE unit actions
            else if ((moveEnd != -1) && (moveBegin == -1) && (moveType != ActionTypes::MOVE))
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
const Action & MoveArray::getMove(const size_t & unit, const size_t & move) const
{
    return _moves[unit][(size_t)move];
}

void MoveArray::printCurrentMoveIndex()
{
    for (size_t u(0); u<numUnits(); ++u)
    {
        std::cout << _currentMovesIndex[u] << " ";
    }

    std::cout << std::endl;
}

void MoveArray::incrementMove(const size_t & unitIndex)
{
    // increment the index for this unit
    _currentMovesIndex[unitIndex] = (_currentMovesIndex[unitIndex] + 1) % _moves[unitIndex].size();

    // if the value rolled over, we need to do the carry calculation
    if (_currentMovesIndex[unitIndex] == 0)
    {
        // the next unit index
        size_t nextUnit = unitIndex + 1;

        // if we have space left to increment, do it
        if (nextUnit < numUnits())
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

    _currentMoves[unitIndex] = _moves[unitIndex][_currentMovesIndex[unitIndex]];
    //_currentMovesVec[unit] = _moves[unit][_currentMovesIndex[unit]];
}

bool MoveArray::hasMoreMoves() const
{
    return _hasMoreMoves;
}

void MoveArray::resetMoveIterator()
{
    _hasMoreMoves = true;
    std::fill(_currentMovesIndex.begin(), _currentMovesIndex.end(), 0);

    for (size_t u(0); u<numUnits(); ++u)
    {
        _currentMoves[u] = _moves[u][_currentMovesIndex[u]];
    }
}

void MoveArray::getNextmove(Move & moves)
{
    moves = _currentMoves;
    //moves = _currentMovesVec;
    incrementMove(0);
}

// adds a Move to the unit specified
void MoveArray::add(const Action & action)
{
    SPARCRAFT_ASSERT(action.type() != ActionTypes::NONE, "Adding NONE action type to MoveArray");

    const size_t unitIndex = getUnitIndex(action.getID());

    if (unitIndex >= _moves.size())
    {
        _moves.push_back(Move());
        _moves.back().addAction(action);
        _currentMoves.addAction(_moves.back()[0]);
        _currentMovesIndex.push_back(0);
    }
    else
    {
        _moves[unitIndex].addAction(action);
    }
}

size_t MoveArray::getUnitIndex(const size_t & unitID) const
{
    for (size_t u(0); u < _moves.size(); ++u)
    {
        if (_moves[u].size() > 0 && _moves[u][0].getID() == unitID)
        {
            return u;
        }
    }

    return _moves.size();
}
	
bool MoveArray::validateMoves()
{
	for (size_t u(0); u<numUnits(); ++u)
	{
		for (size_t m(0); m<numMoves(u); ++m)
		{
			const Action & move(getMove(u, m));

			if (move.getID() > 200)
			{
				printf("Unit Move Incorrect! Something will be wrong\n");
				return false;
			}
		}
	}
		
	return true;
}

const size_t MoveArray::getUnitID(const size_t & unit) const
{
	return getMove(unit, 0).getID();
}

const size_t MoveArray::getPlayerID(const size_t & unit) const
{
	return getMove(unit, 0).getPlayerID();
}

size_t MoveArray::numUnits() const
{ 
    return _moves.size(); 
}

size_t MoveArray::numMoves(const size_t & unitIndex) const
{ 
    SPARCRAFT_ASSERT(unitIndex < _moves.size(), "Trying to get numMoves of a unit that isn't in the MoveArray");

    return _moves[unitIndex].size(); 
}