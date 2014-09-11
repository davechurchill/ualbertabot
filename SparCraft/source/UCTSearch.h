#pragma once

#include <limits>

#include "Timer.h"
#include "GameState.h"
#include "UnitAction.hpp"
#include "GraphViz.hpp"
#include "Array.hpp"
#include "MoveArray.h"
#include "UCTSearchParameters.hpp"
#include "UCTSearchResults.hpp"
#include "Player.h"
#include "AllPlayers.h"
#include "UCTNode.h"
#include "GraphViz.hpp"
#include "UCTMemoryPool.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

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
    std::vector<UnitAction>                 _actionVec;
	MoveArray                               _moveArray;
	Array<std::vector<UnitAction>,
		 Constants::Max_Ordered_Moves>      _orderedMoves;

    std::vector<PlayerPtr>					_allScripts[Constants::Num_Players];
    PlayerPtr                               _playerModels[Constants::Num_Players];

public:

	UCTSearch(const UCTSearchParameters & params);

    
    // UCT-specific functions
    UCTNode &       UCTNodeSelect(UCTNode & parent);
    StateEvalScore  traverse(UCTNode & node, GameState & currentState);
	void            uct(GameState & state, size_t depth, const IDType lastPlayerToMove, std::vector<UnitAction> * firstSimMove);

	void            doSearch(GameState & initialState, std::vector<UnitAction> & move);
    
    // Move and Child generation functions
    void            generateChildren(UCTNode & node, GameState & state);
	void            generateOrderedMoves(GameState & state, MoveArray & moves, const IDType & playerToMove);
    void            makeMove(UCTNode & node, GameState & state);
	const bool      getNextMove(IDType playerToMove, MoveArray & moves, const size_t & moveNumber, std::vector<UnitAction> & actionVec);

    // Utility functions
	const IDType    getPlayerToMove(UCTNode & node, const GameState & state) const;
    const size_t    getChildNodeType(UCTNode & parent, const GameState & prevState) const;
	const bool      searchTimeOut();
	const bool      isRoot(UCTNode & node) const;
	const bool      terminalState(GameState & state, const size_t & depth) const;
    const bool      isFirstSimMove(UCTNode & node, GameState & state);
    const bool      isSecondSimMove(UCTNode & node, GameState & state);
    StateEvalScore  performPlayout(GameState & state);
    void            updateState(UCTNode & node, GameState & state, bool isLeaf);
    void            setMemoryPool(UCTMemoryPool * pool);
    UCTSearchResults & getResults();

    // graph printing functions
    void            printSubTree(UCTNode & node, GameState state, std::string filename);
    void            printSubTreeGraphViz(UCTNode & node, GraphViz::Graph & g, GameState state);
    std::string     getNodeIDString(UCTNode & node);
};
}