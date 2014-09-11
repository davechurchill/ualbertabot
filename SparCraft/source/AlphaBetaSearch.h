#pragma once

#include <limits>

#include "AllPlayers.h"
#include "Timer.h"
#include "GameState.h"
#include "UnitAction.hpp"
#include "Array.hpp"
#include "MoveArray.h"
#include "TranspositionTable.h"
#include "Player.h"

#include "AlphaBetaSearchResults.hpp"
#include "AlphaBetaSearchParameters.hpp"
#include "GraphViz.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

namespace SparCraft
{

class Game;
class AlphaBetaSearchParameters;
class Player;


class AlphaBetaSearch
{
	AlphaBetaSearchParameters               _params;
	AlphaBetaSearchResults                  _results;
	SparCraft::Timer                        _searchTimer;

	size_t                                  _currentRootDepth;

	Array<MoveArray, 
          Constants::Max_Search_Depth>      _allMoves;

	Array2D<std::vector<UnitAction>, 
			Constants::Max_Search_Depth, 
			Constants::Max_Ordered_Moves>   _orderedMoves;

    std::vector<PlayerPtr>					_allScripts[Constants::Num_Players];
    PlayerPtr                               _playerModels[Constants::Num_Players];

	TTPtr                                   _TT;

public:

	AlphaBetaSearch(const AlphaBetaSearchParameters & params, TTPtr TT = TTPtr((TranspositionTable *)NULL));

	void doSearch(GameState & initialState);

	// search functions
	AlphaBetaValue IDAlphaBeta(GameState & initialState, const size_t & maxDepth);
	AlphaBetaValue alphaBeta(GameState & state, size_t depth, const IDType lastPlayerToMove, std::vector<UnitAction> * firstSimMove, StateEvalScore alpha, StateEvalScore beta);

	// Transposition Table
	TTLookupValue TTlookup(const GameState & state, StateEvalScore & alpha, StateEvalScore & beta, const size_t & depth);
	void TTsave(GameState & state, const StateEvalScore & value, const StateEvalScore & alpha, const StateEvalScore & beta, const size_t & depth, 
				const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove);

	// Transposition Table look up + alpha/beta update

	// get the results from the search
	AlphaBetaSearchResults & getResults();
    	
	void generateOrderedMoves(GameState & state, MoveArray & moves, const TTLookupValue & TTval, const IDType & playerToMove, const size_t & depth);
	const IDType getEnemy(const IDType & player) const;
	const IDType getPlayerToMove(GameState & state, const size_t & depth, const IDType & lastPlayerToMove, const bool isFirstSimMove) const;
	bool getNextMoveVec(IDType playerToMove, MoveArray & moves, const size_t & moveNumber, const TTLookupValue & TTval, const size_t & depth, std::vector<UnitAction> & moveVec) const;
	const size_t getNumMoves(MoveArray & moves, const TTLookupValue & TTval, const IDType & playerToMove, const size_t & depth) const;
	const AlphaBetaMove & getAlphaBetaMove(const TTLookupValue & TTval, const IDType & playerToMove) const;
	const bool searchTimeOut();
	const bool isRoot(const size_t & depth) const;
	const bool terminalState(GameState & state, const size_t & depth) const;
	const bool isTranspositionLookupState(GameState & state, const std::vector<UnitAction> * firstSimMove) const;

	void printTTResults() const;
};
}