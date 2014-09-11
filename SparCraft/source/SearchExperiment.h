#pragma once

#include "SparCraft.h"
#include <iomanip>

namespace SparCraft
{
    class SearchExperiment;
}

typedef std::vector<std::string> sv;
typedef std::vector<sv> svv;
typedef std::vector<int> iv;
typedef std::vector<iv> ivv;
typedef std::vector<ivv> ivvv;
typedef std::vector<double> dv;
typedef std::vector<dv> dvv;
typedef std::vector<dvv> dvvv;

class TranspositionTable;

class SparCraft::SearchExperiment
{
	std::vector<PlayerPtr>      players[2];
    std::vector<std::string>    playerStrings[2];
    std::vector<GameState>      states;
    Map *                       map;
    bool                        showDisplay;

    std::string                 resultsFile;
    bool                        appendTimeStamp;
    std::string                 timeString;
    std::string                 configFileFull;
    std::string                 configFileSmall;
    std::string                 imageDir;

	iv                          resultsPlayers[2];
	ivvv                        resultsStateNumber;
	ivvv                        resultsNumUnits;
	ivvv                        resultsEval;
	ivvv                        resultsRounds;
	dvvv                        resultsTime;
    ivv                         numGames;
	ivv                         numWins;
    ivv                         numLosses;
	ivv                         numDraws;

	RandomInt					rand;

    void setupResults();
    void addPlayer(const std::string & line);
    void addState(const std::string & line);
    void padString(std::string & str, const size_t & length);
    void setCurrentDateTime();
    std::string getDateTimeString();
    svv getExpDescription(const size_t & p1, const size_t & p2, const size_t & state);

    std::vector<std::string> getLines(const std::string & filename);

    Position    getRandomPosition(  const PositionType & xlimit, const PositionType & ylimit);

    void   addSeparatedState(	std::vector<std::string> & unitTypes, std::vector<int> & numUnits,
                                    const int cx1, const int cy1, const int cx2, const int cy2,
								    const PositionType & xLimit, const PositionType & yLimit);

    GameState   getSymmetricState(	std::vector<std::string> & unitTypes, std::vector<int> & numUnits,
								    const PositionType & xLimit, const PositionType & yLimit);
    
    GameState   getLineState(       const BWAPI::UnitType type1, const size_t & num1, 
                                    const BWAPI::UnitType type2, const size_t & num2,
                                    const size_t xSpace, const size_t ySpace);

    void        addLineStates(      std::vector<GameState> & states, int num);
    
    std::string getBaseFilename(const std::string & filename);
    BWAPI::UnitType getUnitType(const std::string & unitTypeString);
    void parseStateDescriptionFile(const std::string & fileName);
    void parseConfigFile(const std::string & filename);
    void writeConfig(const std::string & configfile);
	std::string getResultsSummaryFileName();
    std::string getResultsOutFileName();
    std::string getConfigOutFileName();
    std::string currentDateTime();
    void printStateUnits(std::ofstream & results, GameState & state);
    void addGameState(const GameState & state);

public:

    SearchExperiment(const std::string & configFile);
    ~SearchExperiment();

    void runExperiment();
	void writeResultsSummary();
};
