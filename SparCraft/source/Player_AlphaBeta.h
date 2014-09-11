#pragma once

#include "Common.h"
#include "Player.h"

#include "TranspositionTable.h"
#include "AlphaBetaSearch.h"
#include "AlphaBetaSearchParameters.hpp"
#include "AlphaBetaSearchResults.hpp"

#include <boost/shared_ptr.hpp>

namespace SparCraft
{
   
class AlphaBetaSearch;
/*----------------------------------------------------------------------
 | Alpha Beta Player
 |----------------------------------------------------------------------
 | Runs Alpha Beta search given a set of search parameters
 `----------------------------------------------------------------------*/
class Player_AlphaBeta : public Player
{
	AlphaBetaSearch * alphaBeta;
	TTPtr TT;
	AlphaBetaSearchParameters _params;
public:
	Player_AlphaBeta (const IDType & playerID);
	Player_AlphaBeta (const IDType & playerID, const AlphaBetaSearchParameters & params, TTPtr table);
    virtual ~Player_AlphaBeta();
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
	void setParameters(AlphaBetaSearchParameters & p);
	AlphaBetaSearchParameters & getParams();
	void setTranspositionTable(TTPtr table);
	AlphaBetaSearchResults & results();
	IDType getType() { return PlayerModels::AlphaBeta; }
};

}
