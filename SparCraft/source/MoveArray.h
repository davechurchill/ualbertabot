#pragma once

#include "Common.h"
#include "Array.hpp"
#include "Unit.h"
#include "UnitAction.hpp"

namespace SparCraft
{
class MoveArray
{
	// the array which contains all the moves
	Array2D<UnitAction, Constants::Max_Units, Constants::Max_Moves>     _moves;

	// how many moves each unit has
	Array<size_t, Constants::Max_Units>                                 _numMoves;

    // the current move array, used for the 'iterator'
    //std::vector<UnitAction> _currentMoves;
    //std::vector<UnitAction>                                             _currentMovesVec;
    Array<UnitAction, Constants::Max_Units>                             _currentMoves;
    Array<size_t, Constants::Max_Units>                                 _currentMovesIndex;

	// the number of units that have moves;
	size_t                                                              _numUnits;
	size_t                                                              _maxUnits;
    bool                                                                _hasMoreMoves;

public:

	MoveArray(const size_t maxUnits = 0);

	void clear();

	// returns a given move from a unit
	const UnitAction & getMove(const size_t & unit, const size_t & move) const;

    void printCurrentMoveIndex();

    void incrementMove(const size_t & unit);

    const bool hasMoreMoves() const;

    void resetMoveIterator();

    void getNextMoveVec(std::vector<UnitAction> & moves);

	const size_t maxUnits() const;

	// adds a Move to the unit specified
	void add(const UnitAction & move);
	
	bool validateMoves();

	const IDType getUnitID(const IDType & unit) const;

	const IDType getPlayerID(const IDType & unit) const;

	void addUnit();

    void shuffleMoveActions();

	const size_t & numUnits()						const;
	const size_t & numUnitsInTuple()				const;
	const size_t & numMoves(const size_t & unit)	const;
};
}