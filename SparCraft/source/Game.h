#pragma once

#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Action.h"
#include <memory>
#include "Timer.h"

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;

class UnitScriptData;

class Game
{
protected:
	
    GameState           _state;
    PlayerPtr			_players[2];
	size_t				_numPlayers;
    size_t              _numMoves[2];
    size_t              _numActions[2];
	size_t				_playerToMoveMethod;
	size_t				_rounds;
	Timer				_t;
	double				_gameTimeMS;
	size_t				_moveLimit;
    bool                _simulateEveryFrame;

public:
	
	// game constructor
	Game(const GameState & initialState, PlayerPtr p1, PlayerPtr p2, size_t turnLimit = 0);
    Game(const GameState & initialState, size_t limit = 0);

	void                play();
    void                playNextTurn();
	void                storeHistory(const bool & store);
    void                setSimulateEveryFrame(bool sim);
	bool                gameOver() const;
    
	double              eval(const size_t & evalMethod) const;

	GameState &         getState();
    const GameState &   getState() const;
	int                 getRounds();
	double              getTime();
	const size_t        getPlayerToMove();
    PlayerPtr           getPlayer(const size_t & player);
};



}