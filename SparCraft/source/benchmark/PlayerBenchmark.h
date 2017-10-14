#pragma once

#include "../SparCraft.h"

typedef std::vector< std::vector<size_t> > vvs;

namespace SparCraft
{
 
class PlayerBenchmark 
{
    std::string         _benchmarkName;
    std::string         _date;
    PlayerPtr           _players[2];
    std::string         _playerNames[2];
    size_t              _timeLimitSec;
    size_t              _writeIntervalSec;
    size_t              _randomCards;
    size_t              _gamesPlayed;
    size_t              _wins[3];
    size_t              _totalGameTurns;
    double              _timeElapsed;
    GameState           _beginTurnState;
    std::vector<double> _willScore;
    std::vector<double> _willScoreAfterDefense;
    std::vector<size_t> _evaluationsPerformed;
    std::vector<size_t> _depthReached;
    std::vector<size_t> _nodesExpanded;
    std::vector<size_t> _rootChildrenGenerated;
    std::vector<double> _turnTimeElapsed;
    size_t              _willScoreCorrect;
    size_t              _willScoreAfterDefenseCorrect;
    std::vector<size_t> _willScoreCorrectOnTurn;
    std::vector<size_t> _willScoreAfterDefenseCorrectOnTurn;
    std::vector<size_t> _playoutCorrectOnTurn;
    std::vector<size_t> _totalTurnsReached;
    std::vector<size_t> _playoutWinner;
    size_t              _playoutCorrect;

    FILE *              _learningDataFile;

    
    bool                _doClickString;
    bool                _doUnitStats;
    bool                _doEvalStats;
    bool                _doPlayoutEvalStats;
    bool                _doSearchStats;

    // unit statistics data
    vvs                 _gameUnitsBought;
    vvs                 _timesBought;   // v[cardID][player] = num times card was bought by player - total purchases
    vvs                 _gamesBought;   // v[cardID][player] = num games in which card was bought by player
    vvs                 _gamesWon;      // v[cardID][player] = num games won by player when card was bought
    std::vector<size_t> _gamesAvailable;
    
    // functions for keeping track of search statistics
    void updateSearchStatistics(const Game & game);

    // functions for keeping unit purchase statistics
    void clearUnitStatistics(const GameState & state);
    void calculateUnitStatistics(const Game & game);
    void updateUnitStatistics(const Game & game);
    
    // functions for keeping evaluation statistics
    void clearEvaluationStatistics();
    void updateEvaluationStatistics(const Game & game);
    void calculateEvaluationStatistics(const Game & game);

    void doClickStringTest(const Game & game);

    void parseState(const GameState & state);
    void updateUnitBuyStats(const Game & game, const vvs & cardBought);
    std::string getTimeStringFromMS(const size_t ms);

public:
    
    PlayerBenchmark(const rapidjson::Value & value);

    void run();

    
    void printResults();
};
}