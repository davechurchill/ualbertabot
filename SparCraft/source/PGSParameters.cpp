#include "PGSParameters.h"

using namespace SparCraft;

PGSParameters::PGSParameters(const size_t & playerID)
    : _playerID(playerID)
    , _iterations(1)
    , _responses(0)
    , _maxPlayoutTurns(0)
{

}

void PGSParameters::setMaxPlayoutTurns(const size_t & turns)
{
    _maxPlayoutTurns = turns;
}

void PGSParameters::setEnemySeedPlayer(const PlayerPtr & player)
{
    _enemySeedPlayer = player;
}

void PGSParameters::setIterations(const size_t & iterations)
{
    _iterations = iterations;
}

void PGSParameters::setResponses(const size_t & responses)
{
    _responses = responses;
}

void PGSParameters::setTimeLimit(const size_t & timeLimit)
{
    _timeLimit = timeLimit;
}

void PGSParameters::addPortfolioPlayer(const size_t & playerID, const PlayerPtr & player)
{
    _playerPortfolio[playerID].push_back(player);
}

const size_t & PGSParameters::getMaxPlayoutTurns() const
{
    return _maxPlayoutTurns;
}

const PlayerPtr & PGSParameters::getEnemySeedPlayer() const
{
    return _enemySeedPlayer;
}

const size_t & PGSParameters::getTimeLimit() const
{
    return _timeLimit;
}

const size_t & PGSParameters::getPlayer() const
{
    return _playerID;
}

const size_t & PGSParameters::getIterations() const
{
    return _iterations;
}

const size_t & PGSParameters::getResponses() const
{
    return _responses;
}

const std::vector<PlayerPtr> & PGSParameters::getPortfolio(const size_t & playerID) const
{
    return _playerPortfolio[playerID];
}

const std::string & PGSParameters::toString()
{
    if (_description.size() == 0)
    {
        std::stringstream ss;
        ss << "Iterations: " << _iterations << "\n";
        ss << "Responses:  " << _responses << "\n";
        ss << "Max PTurns: " << _maxPlayoutTurns << "\n";
        ss << "Time Limit: " << _timeLimit << "\n";
        ss << "Portfolio:\n";

        for (size_t i(0); i < _playerPortfolio[_playerID].size(); ++i)
        {
            ss << "  " << _playerPortfolio[_playerID][i]->getName() << "\n";
        }

        _description = ss.str();
    }

    return _description;
}