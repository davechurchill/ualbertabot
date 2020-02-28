#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Action.h"
#include "UnitScriptData.h"
#include <memory>
#include "Timer.h"

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;

class UnitScriptData;

class Game
{
protected:
	PlayerPtr			_players[2];
	size_t				_numPlayers;
	size_t				_playerToMoveMethod;
	size_t				rounds;
	Timer				t;
	double				gameTimeMS;
	size_t				moveLimit;

	GameState state;

	// moves array to store moves in
	MoveArray moves[2];
	std::vector<Action> scriptMoves[2];

    

public:
	
	// game constructor
	Game(const GameState & initialState, PlayerPtr & p1, PlayerPtr & p2, const size_t & limit);
    Game(const GameState & initialState, const size_t & limit);

	void            play();
    void            playNextTurn();
    void            playIndividualScripts(UnitScriptData & scriptsChosen);
	void            storeHistory(const bool & store);
	bool            gameOver() const;
    

	ScoreType       eval(const size_t & evalMethod) const;

	GameState &     getState();
    const GameState &     getState() const;
	int             getRounds();
	double          getTime();
	const size_t    getPlayerToMove();
    PlayerPtr       getPlayer(const size_t & player);

};



}