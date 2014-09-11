#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "UnitAction.hpp"
#include "UnitScriptData.h"
#include <boost/shared_ptr.hpp>
#include "Timer.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Display.h"
#endif

namespace SparCraft
{
	
typedef	boost::shared_ptr<Player> PlayerPtr;

class UnitScriptData;

class Game
{
protected:
	PlayerPtr			_players[2];
	size_t				_numPlayers;
	IDType				_playerToMoveMethod;
	size_t				rounds;
	Timer				t;
	double				gameTimeMS;
	size_t				moveLimit;

	GameState state;

	// moves array to store moves in
	MoveArray moves[2];
	std::vector<UnitAction> scriptMoves[2];

public:
	
#ifdef USING_VISUALIZATION_LIBRARIES
	Display *			disp;
#endif

	// game constructor
	Game(const GameState & initialState, PlayerPtr & p1, PlayerPtr & p2, const size_t & limit);
    Game(const GameState & initialState, const size_t & limit);

	void            play();
    void            playIndividualScripts(UnitScriptData & scriptsChosen);
	void            storeHistory(const bool & store);
	bool            gameOver();

	ScoreType       eval(const IDType & evalMethod) const;

	GameState &     getState();
	int             getRounds();
	double          getTime();
	const IDType    getPlayerToMove();

#ifdef USING_VISUALIZATION_LIBRARIES
    void setDisplay(Display * d);
#endif
};



}