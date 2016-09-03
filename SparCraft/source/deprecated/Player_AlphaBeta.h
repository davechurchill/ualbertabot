#pragma once

#include "Common.h"
#include "Player.h"

#include "TranspositionTable.h"
#include "AlphaBetaSearch.h"
#include "AlphaBetaSearchParameters.hpp"
#include "AlphaBetaSearchResults.hpp"

namespace SparCraft
{

class AlphaBetaSearch;
   
class Player_AlphaBeta : public Player
{
	AlphaBetaSearch * alphaBeta;
	TTPtr TT;
	AlphaBetaSearchParameters _params;
public:
	Player_AlphaBeta (const size_t & playerID);
	Player_AlphaBeta (const size_t & playerID, const AlphaBetaSearchParameters & params, TTPtr table);
    virtual ~Player_AlphaBeta();

	void getMove(const GameState & state, Move & move);
	void setParameters(AlphaBetaSearchParameters & p);
	void setTranspositionTable(TTPtr table);

	AlphaBetaSearchParameters & getParams();
	AlphaBetaSearchResults & results();
	virtual PlayerPtr clone();
};

}
