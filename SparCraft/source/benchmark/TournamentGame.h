#pragma once

#include "../SparCraft.h"
#include "../rapidjson/document.h"

namespace SparCraft
{
 
class TournamentGame
{
    GameState       _currentState;
    PlayerPtr       _players[2];
    std::string     _playerNames[2];
    size_t          _playerTotalTimeMS[2];
    size_t          _maxTimeMS[2];
    size_t          _totalMoves[2];
    size_t          _totalActions[2];
        
public:

    TournamentGame(const GameState & initialState, const std::string & p1name, PlayerPtr p1, const std::string & p2name, const PlayerPtr p2);

    void playGame();

    const std::string & getPlayerName(const size_t & player) const;
    const GameState & getFinalGameState() const;
    const size_t getTotalTimeMS(const size_t & player) const;
    const size_t getMaxTimeMS(const size_t & player) const;
    const size_t getTotalMoves(const size_t & player) const;
};

}