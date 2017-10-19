#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{

class PGSParameters
{
    PlayerPtr               _enemySeedPlayer;
    size_t                  _playerID;
    size_t                  _iterations;
    size_t                  _responses;
    size_t                  _timeLimit;
    size_t                  _maxPlayoutTurns;
    std::vector<PlayerPtr>  _playerPortfolio[2];
    std::string             _description;

public:

    PGSParameters(const size_t & playerID);

    void setEnemySeedPlayer(const PlayerPtr & player);
    void setIterations(const size_t & iterations);
    void setResponses(const size_t & responses);
    void setTimeLimit(const size_t & timeLimit);
    void setMaxPlayoutTurns(const size_t & turns);
    void addPortfolioPlayer(const size_t & playerID, const PlayerPtr & player);

    const PlayerPtr & getEnemySeedPlayer() const;
    const size_t & getPlayer() const;
    const size_t & getIterations() const;
    const size_t & getResponses() const;
    const size_t & getTimeLimit() const;
    const size_t & getMaxPlayoutTurns() const;
    const std::vector<PlayerPtr> & getPortfolio(const size_t & playerID) const;

    const std::string & toString();
};

}