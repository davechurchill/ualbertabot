#pragma once

#include "Common.h"
#include "Array.hpp"
#include "Unit.h"
#include "Action.h"
#include "Move.h"

namespace SparCraft
{
class MoveArray
{
	// the array which contains all the moves
    std::vector< Move > _moves;
	//Array2D<Action, Constants::Max_Units, Constants::Max_Moves> _moves;

	// how many moves each unit has
	//Array<size_t, Constants::Max_Units>                         _numMoves;

    // the current move array, used for the 'iterator'
    //Move _currentMoves;
    //Move                                             _currentMovesVec;
    Move                             _currentMoves;
    std::vector<size_t>                             _currentMovesIndex;

	// the number of units that have moves;
    bool                                            _hasMoreMoves;

public:

	MoveArray();

	void clear();

	// returns a given move from a unit
	const Action & getMove(const size_t & unit, const size_t & move) const;
    bool hasMoreMoves() const;
    size_t getUnitIndex(const size_t & unitID) const;

    void getNextmove(Move & moves);
    void resetMoveIterator();
    void incrementMove(const size_t & unit);
    void printCurrentMoveIndex();

	const size_t maxUnits() const;

	// adds a Move to the unit specified
	void add(const Action & move);
	
	bool validateMoves();

	const size_t getUnitID(const size_t & unit) const;
	const size_t getPlayerID(const size_t & unit) const;

    void shuffleMoveActions();

	size_t numUnits() const;
	size_t numMoves(const size_t & unit) const;
};
}