#pragma once

#include <limits>

#include "Timer.h"
#include "GameState.h"
#include "Action.h"
#include "GraphViz.hpp"
#include "MoveArray.h"
#include "UCTSearchParameters.hpp"
#include "UCTSearchResults.hpp"
#include "Player.h"
#include "AllPlayers.h"
#include "UCTNode.h"
#include "GraphViz.hpp"
#include "UCTMemoryPool.hpp"
#include "Eval.h"
#include <memory>

namespace SparCraft
{

class Game;
class Player;

class UCTSearch
{
	UCTSearchParameters 	_params;
    UCTSearchResults        _results;
	Timer		            _searchTimer;
    UCTNode                 _rootNode;
    UCTMemoryPool *         _memoryPool;

    GameState               _currentState;

	// we will use these as variables to save stack allocation every time
    Move                 _actionVec;
	MoveArray                           _moveArray;
	std::vector<Move>   _orderedMoves;

    std::vector<PlayerPtr>				_allScripts[Constants::Num_Players];
    PlayerPtr                           _playerModels[Constants::Num_Players];

public:

	UCTSearch(const UCTSearchParameters & params);

    
    // UCT-specific functions
    UCTNode &       UCTNodeSelect(UCTNode & parent);
    StateEvalScore  traverse(UCTNode & node, GameState & currentState);
	void            uct(GameState & state, size_t depth, const size_t lastPlayerToMove, Move * firstSimMove);

	void            doSearch(const GameState & initialState, Move & move);
    
    // Move and Child generation functions
    void            generateChildren(UCTNode & node, GameState & state);
	void            generateOrderedMoves(GameState & state, const size_t & playerToMove);
    void            makeMove(const UCTNode & node, GameState & state);
	const bool      getNextMove(size_t playerToMove, MoveArray & moves, const size_t & moveNumber, Move & actionVec);

    // Utility functions
	const size_t    getPlayerToMove(const UCTNode & node, const GameState & state) const;
    const size_t    getChildNodeType(const UCTNode & parent, const GameState & prevState) const;
	const bool      searchTimeOut();
	const bool      isRoot(const UCTNode & node) const;
	const bool      terminalState(GameState & state, const size_t & depth) const;
    const bool      isFirstSimMove(const UCTNode & node, GameState & state);
    const bool      isSecondSimMove(const UCTNode & node, GameState & state);
    StateEvalScore  performPlayout(const GameState & state);
    void            updateState(UCTNode & node, GameState & state, bool isLeaf);
    void            setMemoryPool(UCTMemoryPool * pool);
    UCTSearchResults & getResults();

    // graph printing functions
    void            printSubTree(const UCTNode & node, GameState state, std::string filename);
    void            printSubTreeGraphViz(const UCTNode & node, GraphViz::Graph & g, GameState state);
    std::string     getNodeIDString(const UCTNode & node);
};
}