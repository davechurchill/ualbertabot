#pragma once

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
	PlayerID				_playerToMoveMethod;
	size_t				_rounds;
	Timer				_t;
	double				_gameTimeMS;
	size_t				_moveLimit;

	GameState           _state;

	// moves array to store moves in
	//MoveArray           _moves[2];
	//std::vector<Action> _scriptMoves[2];

public:
	
	// game constructor
	Game(const GameState & initialState, PlayerPtr p1, PlayerPtr p2, size_t turnLimit = 0);
    Game(const GameState & initialState, size_t limit = 0);

	void                play();
    void                playNextTurn();
    void                playIndividualScripts(UnitScriptData & scriptsChosen);
	void                storeHistory(const bool & store);
	bool                gameOver() const;
    
	ScoreType           eval(const PlayerID & evalMethod) const;

	GameState &         getState();
    const GameState &   getState() const;
	int                 getRounds();
	double              getTime();
	const PlayerID        getPlayerToMove();
    PlayerPtr           getPlayer(const PlayerID & player);

};



}