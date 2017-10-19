#pragma once

#include <limits>

#include "AllPlayers.h"
#include "Timer.h"
#include "GameState.h"
#include "Action.h"
#include "MoveArray.h"
#include "TranspositionTable.h"
#include "Player.h"

#include "AlphaBetaSearchResults.hpp"
#include "AlphaBetaSearchParameters.hpp"
#include "GraphViz.hpp"

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

	std::vector<MoveArray>      _allMoves;

    std::vector< std::vector<Move> >    _orderedMoves;
    std::vector<PlayerPtr>					_allScripts[Constants::Num_Players];
    PlayerPtr                               _playerModels[Constants::Num_Players];

	TTPtr                                   _TT;

    
	void                    generateOrderedMoves(const GameState & state, const TTLookupValue & TTval, const size_t & playerToMove, const size_t & depth);

public:

	AlphaBetaSearch(const AlphaBetaSearchParameters & params, TTPtr TT = TTPtr((TranspositionTable *)NULL));

	void                    doSearch(const GameState & initialState);

	// search functions
	AlphaBetaValue          IDAlphaBeta(const GameState & initialState, const size_t & maxDepth);
	AlphaBetaValue          alphaBeta(const GameState & state, size_t depth, const size_t lastPlayerToMove, Move * firstSimMove, StateEvalScore alpha, StateEvalScore beta);

	// Transposition Table
	TTLookupValue           TTlookup(const GameState & state, StateEvalScore & alpha, StateEvalScore & beta, const size_t & depth);
	void                    TTsave(const GameState & state, const StateEvalScore & value, const StateEvalScore & alpha, const StateEvalScore & beta, const size_t & depth, 
				                    const size_t & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove);

	// get the results from the search
	AlphaBetaSearchResults & getResults();
    	
	const size_t            getEnemy(const size_t & player) const;
	const size_t            getPlayerToMove(const GameState & state, const size_t & depth, const size_t & lastPlayerToMove, const bool isFirstSimMove) const;
	bool                    getNextmove(size_t playerToMove, MoveArray & moves, const size_t & moveNumber, const TTLookupValue & TTval, const size_t & depth, Move & move) const;
	const size_t            getNumMoves(MoveArray & moves, const TTLookupValue & TTval, const size_t & playerToMove, const size_t & depth) const;
	const AlphaBetaMove &   getAlphaBetaMove(const TTLookupValue & TTval, const size_t & playerToMove) const;
	const bool              searchTimeOut();
	const bool              isRoot(const size_t & depth) const;
	const bool              terminalState(const GameState & state, const size_t & depth) const;
	const bool              isTranspositionLookupState(const GameState & state, const Move * firstSimMove) const;
    
	void                    printTTResults() const;
};
}