#include "PlayerBenchmark.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace SparCraft;

PlayerBenchmark::PlayerBenchmark(const rapidjson::Value & value)
    : _gamesPlayed(0)
    , _totalGameTurns(0)
    , _timeLimitSec(0)
    , _timeElapsed(0)
    , _doClickString(false)
    , _doUnitStats(false)
    , _doPlayoutEvalStats(false)
    , _doSearchStats(false)
    , _doEvalStats(false)
{
    SPARCRAFT_ASSERT(value.HasMember("name") && value["name"].IsString(), "PlayerBenchmark must have Name string");
    SPARCRAFT_ASSERT(value.HasMember("PlayerOne") && value["PlayerOne"].IsString(), "PlayerBenchmark must have PlayerOne string");
    SPARCRAFT_ASSERT(value.HasMember("PlayerTwo") && value["PlayerTwo"].IsString(), "PlayerBenchmark must have PlayerTwo string");
    SPARCRAFT_ASSERT(value.HasMember("TimeLimitSec") && value["TimeLimitSec"].IsInt(), "PlayerBenchmark must have TimeLimitMS int");
    SPARCRAFT_ASSERT(value.HasMember("UpdateIntervalSec") && value["UpdateIntervalSec"].IsInt(), "PlayerBenchmark must have UpdateIntervalMS int");

    JSONTools::ReadString("name", value, _benchmarkName);
    JSONTools::ReadString("PlayerOne", value, _playerNames[Players::Player_One]);
    JSONTools::ReadString("PlayerTwo", value, _playerNames[Players::Player_Two]);
    JSONTools::ReadInt("TimeLimitSec", value, _timeLimitSec);
    JSONTools::ReadInt("RandomCards", value, _randomCards);
    JSONTools::ReadInt("UpdateIntervalSec", value, _writeIntervalSec);

    _players[Players::Player_One] = AIParameters::Instance().getPlayer(Players::Player_One, _playerNames[Players::Player_One]);
    _players[Players::Player_Two] = AIParameters::Instance().getPlayer(Players::Player_Two, _playerNames[Players::Player_Two]);

    _wins[0] = 0;
    _wins[1] = 0;
    _wins[2] = 0;
}

void PlayerBenchmark::run()
{
    auto time = std::time(nullptr);
    auto tm = *std::localtime(&time);

    std::stringstream startDate;
    startDate << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    _date = startDate.str();
    
    std::cout << "\nStarting " << _timeLimitSec << "sec PlayerBenchmark: [" << _playerNames[0] << " vs. " << _playerNames[1] << "] with " << _randomCards << " random cards for "  << SPARCRAFT::endl;
    
    printf("\n  TotalGames  TotalTurns  TimeElapsed    Games/Sec   Turns/Game    Turns/Sec     P1Wins     P2Wins      Draws\n");
    printf("  -------------------------------------------------------------------------------------------------------------\n");

    Timer t;
    t.start();

    // keep playing games until the time limit is reached
    while(_timeElapsed < 1000*_timeLimitSec)
    {
        GameState state;
        state.setStartingState(Players::Player_One, _randomCards);

        // initialize all the data structures required for keeping various statistics
        clearUnitStatistics(state);
        clearEvaluationStatistics();

        // set up the game and play it
        Game game(state, _players[0]->clone(), _players[1]->clone());
        while(!game.gameOver())
        {
            if (_doClickString)
            {
                _beginTurnState = game.getState();       
            }

            updateEvaluationStatistics(game);
            updateUnitStatistics(game);
            updateSearchStatistics(game);

            game.playNextTurn();
            _totalGameTurns++;

            // update the various statistics we are collecting
            doClickStringTest(game);
        }

        // record the end of game statistics
        _gamesPlayed++;
        _wins[game.getState().winner()]++;

        // calculate the final statistics for this game
        calculateEvaluationStatistics(game);
        calculateUnitStatistics(game);

        double ms = t.getElapsedTimeInMilliSec();
        if (ms > 1000*_writeIntervalSec)
        {
            _timeElapsed += ms;

            #if !defined(EMSCRIPTEN)
                printResults();
            #endif
            t.start();
            
            printf("%12d%12d%13.2lf%13.2lf%13.2lf%13.2lf%11d%11d%11d\n", _gamesPlayed, _totalGameTurns, _timeElapsed, ((_gamesPlayed / _timeElapsed) * 1000), ((double)_totalGameTurns / _gamesPlayed), ((_totalGameTurns / _timeElapsed) * 1000), _wins[0], _wins[1], (_gamesPlayed - _wins[0] - _wins[1]));
        }
    }
    printResults();
    //fclose(_learningDataFile);
}

void PlayerBenchmark::updateUnitStatistics(const Game & game)
{
    if (!_doUnitStats)
    {
        return;
    }

    const Move & previousMove = game.getPreviousMove();
    for (size_t a(0); a < previousMove.size(); ++a)
    {
        const Action & action = previousMove.getAction(a);

        if (action.getType() == ActionTypes::BUY)
        {
            _gameUnitsBought[action.getID()][action.getPlayer()]++;
        }
    }
}
void PlayerBenchmark::updateSearchStatistics(const Game & game)
{
    if (!_doSearchStats)
    {
        return;
    }

    size_t previousPlayer = game.getState().getInactivePlayer();

    Player * player = game.getPlayer(previousPlayer).get();
    Player_StackAlphaBeta * abPlayer = dynamic_cast<Player_StackAlphaBeta *>(player);
    Player_UCT * uctPlayer = dynamic_cast<Player_UCT *>(player);

    if (_evaluationsPerformed.size() < game.getState().getTurnNumber()+1u)
    {
        _nodesExpanded.resize(game.getState().getTurnNumber()+1, 0);
        _evaluationsPerformed.resize(game.getState().getTurnNumber()+1, 0);
        _rootChildrenGenerated.resize(game.getState().getTurnNumber()+1, 0);
        _turnTimeElapsed.resize(game.getState().getTurnNumber()+1, 0);
    }

    // if this is an alpha beta player
    if (abPlayer != nullptr)
    {
        AlphaBetaSearchResults & results = abPlayer->getResults();

        _evaluationsPerformed[game.getState().getTurnNumber()] += results.evaluationsPerformed;
        _nodesExpanded[game.getState().getTurnNumber()] += results.nodesExpanded;
        _rootChildrenGenerated[game.getState().getTurnNumber()] += results.rootNumChildren;
        _turnTimeElapsed[game.getState().getTurnNumber()] += results.totalTimeElapsed;
    }
    // if it's a uct player
    else if (uctPlayer != nullptr)
    {
        UCTSearchResults & results = uctPlayer->getResults();

        _evaluationsPerformed[game.getState().getTurnNumber()] += results.traversals;
        _nodesExpanded[game.getState().getTurnNumber()] += results.nodesCreated;
        _turnTimeElapsed[game.getState().getTurnNumber()] += results.timeElapsed;
    }
    else
    {
    
    }
}

void PlayerBenchmark::clearEvaluationStatistics()
{
    _willScore.clear();
    _willScoreAfterDefense.clear();
    _playoutWinner.clear();
    _productionDiff.clear();
}

#include <iostream>
#include <iomanip>
#include <ctime>
#include "../htmltools/HTMLTable.h"
#include "../htmltools/HTMLChart.h"
void PlayerBenchmark::printResults()
{
    std::string filename = "html/Benchmark_" + _benchmarkName + "_" + _date + ".html";
    const auto & allCards = CardTypes::GetAllCardTypes();
    
    std::string assertLevel = "No Asserts";

#ifdef SPARCRAFT_ASSERT_NORMAL
    assertLevel = "Normal Asserts";
#endif

#ifdef SPARCRAFT_ASSERT_ALL
    assertLevel = "All Asserts";
#endif
    
    std::stringstream ss;

    ss << "<table cellpadding=2 rules=all style=\"font: 12px/1.5em Verdana; border: 1px solid #cccccc;\">\n";
    ss << "<tr><td width=150><b>Benchmark Name</b></td><td width=200 align=right>" << _benchmarkName << "</td></tr>\n";
    ss << "<tr><td><b>Date Started</b></td><td align=right>" << _date << "</td></tr>\n";
    ss << "<tr><td><b>AI Compiled</b></td><td align=right>" << __DATE__ << " " __TIME__ << "</td></tr>";
    ss << "<tr><td><b>Assert Level</b></td><td align=right>" << assertLevel << "</td></tr>";
    ss << "<tr><td><b>Clickstring Tested</b></td><td align=right>" << (_doClickString ? "true" : "false") << "</td></tr>\n";
    ss << "<tr><td><b>Unit Statistics</b></td><td align=right>" << (_doUnitStats ? "true" : "false") << "</td></tr>\n";
    ss << "<tr><td><b>Player One</b></td><td align=right>" << _playerNames[0] << "</td></tr>\n";
    ss << "<tr><td><b>Player Two</b></td><td align=right>" << _playerNames[1] << "</td></tr>\n";
    ss << "<tr><td><b>Test Time Limit</b></td><td align=right>" << getTimeStringFromMS(_timeLimitSec*1000) << "</td></tr>\n";
    ss << "<tr><td><b>Test Time Elapsed</b></td><td align=right>" << getTimeStringFromMS(_timeElapsed) << "</td></tr>\n";
    ss << "<tr><td><b>Games Played</b></td><td align=right>" << _gamesPlayed << " (" << (1000.0 * _gamesPlayed / _timeElapsed) << "/s)</td></tr>\n";
    ss << "<tr><td><b>Total Turns</b></td><td align=right>" << _totalGameTurns << " (" << (1000.0 * _totalGameTurns / _timeElapsed) << "/s)</td></tr>\n";
    ss << "<tr><td><b>Will Score Correct</b></td><td align=right>" << ((double)_willScoreCorrect / _totalGameTurns) << "</td></tr>\n";
    ss << "<tr><td><b>Score After Def Correct</b></td><td align=right>" << ((double)_willScoreAfterDefenseCorrect / _totalGameTurns) << "</td></tr>\n";
    ss << "<tr><td><b>Playout Correct</b></td><td align=right>" << ((double)_playoutCorrect / _totalGameTurns) << "</td></tr>\n";
    ss << "<tr><td><b>Player One Wins</b></td><td align=right>" << _wins[0] << " - " << (100.0*_wins[0] / _gamesPlayed) << "%%</td></tr>\n";
    ss << "<tr><td><b>Player Two Wins</b></td><td align=right>" << _wins[1] << " - " << (100.0*_wins[1] / _gamesPlayed) << "%%</td></tr>\n";
    ss << "<tr><td><b>Draws</td><td align=right>" << (_gamesPlayed - _wins[0] - _wins[1]) << "</td></tr>\n";
    ss << "</table>\n<br><br>\n";

    FILE * f = fopen(filename.c_str(), "w");
    fprintf(f, "<html>\n<head>\n");
    fprintf(f, "<script type=\"text/javascript\" src=\"javascript/jquery-1.10.2.min.js\"></script>\n<script type=\"text/javascript\" src=\"javascript/jquery.tablesorter.js\"></script>\n<link rel=\"stylesheet\" href=\"javascript/themes/blue/style.css\" type=\"text/css\" media=\"print, projection, screen\" />\n");
    fprintf(f, "</head>\n");
    fprintf(f, ss.str().c_str());
    fclose(f);
    
    if (_doEvalStats)
    {
        HTMLChart chart("Evaluation Accuracy", "Turn Number", "Percent Correct at Turn T");

        std::vector<double> wsPerc(_willScoreCorrectOnTurn.size(), 0);
        std::vector<double> wsadPerc(_willScoreCorrectOnTurn.size(), 0);
        std::vector<double> playoutPerc(_willScoreCorrectOnTurn.size(), 0);
        std::vector<double> goldPerc(_willScoreCorrectOnTurn.size(), 0);
        std::vector<double> attackPerc(_willScoreCorrectOnTurn.size(), 0);
        for (size_t i(0); i < _willScoreCorrectOnTurn.size(); ++i)
        {
            wsPerc[i] = ((double)_willScoreCorrectOnTurn[i] / _totalTurnsReached[i]);
            wsadPerc[i] = ((double)_willScoreAfterDefenseCorrectOnTurn[i] / _totalTurnsReached[i]);
            playoutPerc[i] = ((double)_playoutCorrectOnTurn[i] / _totalTurnsReached[i]);
            goldPerc[i] = ((double)_productionDiffCorrectOnTurn[i].gold / _totalTurnsReached[i]);
            attackPerc[i] = ((double)_productionDiffCorrectOnTurn[i].a / _totalTurnsReached[i]);
        }

        chart.setData("Will Score Inf", wsPerc);
        chart.setData("Will Score NoInf", wsadPerc);
        chart.setData("Gold Income Diff", goldPerc);
        chart.setData("Attack Diff", attackPerc);

        if (_doPlayoutEvalStats)
        {
            chart.setData("Playout", playoutPerc);
        }

        chart.appendHTMLChartToFile(filename, "EvalChart", 0, 50);
    }

    if (_doSearchStats)
    {
        HTMLChart playoutChart("Average Playouts Performed by Turn Number", "Turn Number", "Playouts Performed");
        std::vector<double> avgEvals(_evaluationsPerformed.size(), 0);
        std::vector<double> avgNodes(_nodesExpanded.size(), 0);
        for (size_t i(0); i < _evaluationsPerformed.size(); ++i)
        {
            avgEvals[i] = (double)_evaluationsPerformed[i] / _totalTurnsReached[i];
            avgNodes[i] = (double)_nodesExpanded[i] / _totalTurnsReached[i];
        }
        playoutChart.setData("Evaluations Performed", avgEvals);
        playoutChart.setData("Nodes Expanded", avgNodes);
            
        playoutChart.appendHTMLChartToFile(filename, "PlayoutChart", 0, 50);

        HTMLChart childChart("Average Children Generated At Root", "Turn Number", "Avg Children");
        std::vector<double> avgChildren(_rootChildrenGenerated.size(), 0);
        for (size_t i(0); i < _rootChildrenGenerated.size(); ++i)
        {
            avgChildren[i] = (double)_rootChildrenGenerated[i] / _totalTurnsReached[i];
        }
        childChart.setData("Num Children", avgChildren);
        childChart.appendHTMLChartToFile(filename, "ChildChart", 0, 50);

        HTMLChart timeChart("Average Think Time Elapsed Per Turn", "Turn Number", "Avg Time");
        std::vector<double> avgTime(_turnTimeElapsed.size(), 0);
        for (size_t i(0); i < _turnTimeElapsed.size(); ++i)
        {
            avgChildren[i] = (double)_turnTimeElapsed[i] / _totalTurnsReached[i];
        }
        timeChart.setData("Avg Time Elapsed (ms)", avgChildren);
        timeChart.appendHTMLChartToFile(filename, "TimeChart", 0, 50);
    }
    
    if (_doUnitStats)
    {
        HTMLTable table("Unit Usage Statistics");
        table.setHeader(    {"Unit Name", "id", "$", "R", "G", "B", "Games", "BuyP1", "BuyP2", "AvgBuyP1", "AvgBuyP2", "GBuyP1", "GBuyP2", "PBuyP1", "PBuyP2", "WinP1", "WinP2", "PWonP1", "PWonP2", "PWinDiff"});
        table.setColWidth(  {150,         30,   30,  30,  30,  30,  90,     90,     90,     90,         90,       90,      90,      90,      90,      90,     90,     90,      90,      90});
    
        for (size_t c(2); c < allCards.size(); ++c)
        {
            const Mana & buyCost = allCards[c].getBuyCost();

            size_t col = 0;
            table.setData(c, col++, allCards[c].getUIName().c_str());
            table.setData(c, col++, c);
            table.setData(c, col++, (size_t)buyCost.amountOf(C::MANA_GOLD));
            table.setData(c, col++, (size_t)buyCost.amountOf(C::MANA_RED));
            table.setData(c, col++, (size_t)buyCost.amountOf(C::MANA_GREEN));
            table.setData(c, col++, (size_t)buyCost.amountOf(C::MANA_BLUE));
            table.setData(c, col++, _gamesAvailable[c]);
            table.setData(c, col++, _timesBought[c][0]);
            table.setData(c, col++, _timesBought[c][1]);
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_timesBought[c][0]/_gamesAvailable[c]));
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_timesBought[c][1]/_gamesAvailable[c]));
            table.setData(c, col++, _gamesBought[c][0]);
            table.setData(c, col++, _gamesBought[c][1]);
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_gamesBought[c][0]/_gamesAvailable[c]));
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_gamesBought[c][1]/_gamesAvailable[c]));
            table.setData(c, col++, _gamesWon[c][0]);
            table.setData(c, col++, _gamesWon[c][1]);
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_gamesWon[c][0]/_gamesBought[c][0]));
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : ((double)_gamesWon[c][1]/_gamesBought[c][1]));
            table.setData(c, col++, _gamesAvailable[c] == 0 ? 0 : fabs(((double)_gamesWon[c][1]/_gamesBought[c][1]) - ((double)_gamesWon[c][0]/_gamesBought[c][0])));
        }
            
        table.appendHTMLTableToFile(filename, "unitStatsTable");
    }
}

std::string PlayerBenchmark::getTimeStringFromMS(const size_t ms)
{
    size_t totalSec = ms / 1000;

    size_t sec = totalSec % 60;
    size_t min = (totalSec / 60) % 60;
    size_t hour = (totalSec / 3600);

    std::stringstream ss;
    if (hour > 0)
    {
        ss << hour << "h ";
    }
    if (min > 0)
    {
        ss << min << "m ";
    }

    ss << sec << "s";
    return ss.str();
}
