#pragma once

#include "../SparCraft.h"
#include "../rapidjson/document.h"
#include "TournamentGame.h"

namespace SparCraft
{
 
class Tournament
{
    std::string                         _name;
    std::string                         _type;
    std::string                         _date;
    std::string                         _stateName;
    size_t                              _rounds;
    size_t                              _totalGamesPlayed;
    size_t                              _updateIntervalSec;
    Timer                               _timeElapsed;

    std::vector<std::string>            _players;
    std::vector<std::string>            _stateDescriptions;
    std::vector<int>                    _playerGroups;
    std::vector<int>                    _totalGames;
    std::vector<int>                    _totalWins;
    std::vector<int>                    _totalDraws;
    std::vector<int>                    _totalPlayouts;
    std::vector<int>                    _totalTurns;
    std::vector<int>                    _maxTimeMS;
    std::vector<int>                    _totalTimeMS;
    std::vector< std::vector<int> >     _numGames;
    std::vector< std::vector<int> >     _wins;
    std::vector< std::vector<int> >     _draws;
    std::vector< std::vector<int> >     _turns;

    int getPlayerIndex(const std::string & playerName) const;
    void parseResult(std::string & result);
    void parseTournamentGameResult(const TournamentGame & game);
    void playGame(TournamentGame & game);
    void writeHTMLResults();
    void printResults() const;
    std::string getTimeStringFromMS(const size_t ms);

public:

    Tournament();
    void run(const rapidjson::Value & tournamentValue, const rapidjson::Value & rootValue);

};

}