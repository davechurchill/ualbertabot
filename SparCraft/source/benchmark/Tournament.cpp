#include "Tournament.h"
#include "../ConfigTools.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace SparCraft;

Tournament::Tournament()
    : _totalGamesPlayed(0)
    , _updateIntervalSec(0)
{
    
}

void Tournament::run(const rapidjson::Value & tournamentValue, const rapidjson::Value & rootValue)
{
    SPARCRAFT_ASSERT(tournamentValue.HasMember("Name"), "Tournament has no name");
    SPARCRAFT_ASSERT(tournamentValue.HasMember("Rounds"), "Tournament has no rounds number");
    SPARCRAFT_ASSERT(tournamentValue.HasMember("Players"), "Tournament has no players");

    JSONTools::ReadString("Name", tournamentValue, _name);
    JSONTools::ReadInt("Rounds", tournamentValue, _rounds);
    JSONTools::ReadString("State", tournamentValue, _stateName);
    JSONTools::ReadInt("UpdateIntervalSec", tournamentValue, _updateIntervalSec);

    SPARCRAFT_ASSERT(tournamentValue["Players"].Size() >= 2, "Tournament has less than 2 players");

    for (size_t i(0); i < tournamentValue["Players"].Size(); ++i)
    {
        _players.push_back(tournamentValue["Players"][i]["Name"].GetString());
        _playerGroups.push_back(tournamentValue["Players"][i]["Group"].GetInt());
    }

    auto time = std::time(nullptr);
    auto tm = *std::localtime(&time);

    std::stringstream startDate;
    startDate << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    _date = startDate.str();

    _totalGames = std::vector<int>(_players.size(), 0);
    _totalWins = std::vector<int>(_players.size(), 0);
    _totalDraws = std::vector<int>(_players.size(), 0);
    _totalTurns = std::vector<int>(_players.size(), 0);
    _totalPlayouts = std::vector<int>(_players.size(), 0);
    _totalTimeMS = std::vector<int>(_players.size(), 0);
    _maxTimeMS = std::vector<int>(_players.size(), 0);
    _numGames = std::vector< std::vector<int> >(_players.size(), std::vector<int>(_players.size(), 0));
    _wins = std::vector< std::vector<int> >(_players.size(), std::vector<int>(_players.size(), 0));
    _draws = std::vector< std::vector<int> >(_players.size(), std::vector<int>(_players.size(), 0));
    _turns = std::vector< std::vector<int> >(_players.size(), std::vector<int>(_players.size(), 0));

    Timer t;
    t.start();
    _timeElapsed.start();

    for (size_t r(0); r < _rounds; ++r)
    {
        GameState state = ConfigTools::GetStateFromVariable(_stateName, rootValue);

        for (size_t p1(0); p1 < _players.size(); ++p1)
        {
            for (size_t p2(0); p2 < _players.size(); ++p2)
            {
                if (_playerGroups[p1] == _playerGroups[p2])
                {
                    continue;
                }

                PlayerPtr w1 = AIParameters::Instance().getPlayer(Players::Player_One, _players[p1]);
                PlayerPtr b1 = AIParameters::Instance().getPlayer(Players::Player_Two, _players[p2]);
                PlayerPtr w2 = AIParameters::Instance().getPlayer(Players::Player_One, _players[p2]);
                PlayerPtr b2 = AIParameters::Instance().getPlayer(Players::Player_Two, _players[p1]);

                TournamentGame g1(state, _players[p1], w1, _players[p2], b1);
                TournamentGame g2(state, _players[p2], w2, _players[p1], b2);

                playGame(g1);
                playGame(g2);

                if (t.getElapsedTimeInSec() > _updateIntervalSec)
                {
                    printResults();
                    writeHTMLResults();
                    printf("\n\n");
                    t.start();
                }
            }
        }
    }
}

void Tournament::playGame(TournamentGame & game)
{
    game.playGame();
    parseTournamentGameResult(game);

    _totalGamesPlayed++;
}

void Tournament::parseTournamentGameResult(const TournamentGame & game)
{
    int winnerID = game.getFinalGameState().winner();
    int loserID = (game.getFinalGameState().winner() + 1) % 2;

    int playerIndex[2] = {getPlayerIndex(game.getPlayerName(0)), getPlayerIndex(game.getPlayerName(1))};

    _maxTimeMS[playerIndex[0]] = std::max(_maxTimeMS[playerIndex[0]], (int)game.getMaxTimeMS(0));
    _maxTimeMS[playerIndex[1]] = std::max(_maxTimeMS[playerIndex[1]], (int)game.getMaxTimeMS(1));
    _totalTimeMS[playerIndex[0]] += game.getTotalTimeMS(0);
    _totalTimeMS[playerIndex[1]] += game.getTotalTimeMS(1);
    _totalGames[playerIndex[0]]++;
    _totalGames[playerIndex[1]]++;
    _numGames[playerIndex[0]][playerIndex[1]]++;
    _numGames[playerIndex[1]][playerIndex[0]]++;
    _totalTurns[playerIndex[0]] += game.getTotalMoves(0);
    _totalTurns[playerIndex[1]] += game.getTotalMoves(1);
    _turns[playerIndex[0]][playerIndex[1]] += game.getTotalMoves(0) + game.getTotalMoves(1);
    _turns[playerIndex[1]][playerIndex[0]] += game.getTotalMoves(0) + game.getTotalMoves(1);


    // case of a draw
    if (winnerID == Players::Player_None)
    {
        _draws[playerIndex[0]][playerIndex[1]]++;
        _draws[playerIndex[1]][playerIndex[0]]++;
        _totalDraws[playerIndex[0]]++;
        _totalDraws[playerIndex[1]]++;
    }
    else
    {
        // case of a non-draw
        int winnerIndex = playerIndex[winnerID];
        int loserIndex = playerIndex[loserID];

        _totalWins[winnerIndex]++;
        _wins[winnerIndex][loserIndex]++;
    }
}

#include "../htmltools/HTMLTable.h"
void Tournament::writeHTMLResults()
{
    std::string filename = "html/Tournament_" + _name + "_" + _date + ".html";
        
    std::stringstream ss;
    double timeElapsed = _timeElapsed.getElapsedTimeInMilliSec();

    ss << "<table cellpadding=2 rules=all style=\"font: 12px/1.5em Verdana; border: 1px solid #cccccc;\">\n";
    ss << "<tr><td width=150><b>Tournament Name</b></td><td width=200 align=right>" << _name << "</td></tr>\n";
    ss << "<tr><td><b>Date Started</b></td><td align=right>" << _date << "</td></tr>\n";
    ss << "<tr><td><b>AI Compiled</b></td><td align=right>" << __DATE__ << " " __TIME__ << "</td></tr>";
    ss << "<tr><td><b>Tournament Rounds</b></td><td align=right>" << _rounds << "</td></tr>\n";
    ss << "<tr><td><b>Time Elapsed</b></td><td align=right>" << getTimeStringFromMS(timeElapsed) << "</td></tr>\n";
    ss << "<tr><td><b>Games Played</b></td><td align=right>" << _totalGamesPlayed << " (" << (1000.0 * _totalGamesPlayed / timeElapsed) << "/s)</td></tr>\n";
    ss << "</table>\n<br><br>\n";

    FILE * f = fopen(filename.c_str(), "w");
    fprintf(f, "<html>\n<head>\n");
    fprintf(f, "<script type=\"text/javascript\" src=\"javascript/jquery-1.10.2.min.js\"></script>\n<script type=\"text/javascript\" src=\"javascript/jquery.tablesorter.js\"></script>\n<link rel=\"stylesheet\" href=\"javascript/themes/blue/style.css\" type=\"text/css\" media=\"print, projection, screen\" />\n");
    fprintf(f, "</head>\n");
    fprintf(f, ss.str().c_str());
    fclose(f);

    HTMLTable stats("Overall Statistics");
    stats.setHeader({"Player", "Score", "Games", "Wins", "Loss", "Draw", "Turns", "Turns/G", "MS/Turn", "Max MS"});
    stats.setColWidth({120, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80});

    for (size_t p(0); p < _players.size(); ++p)
    {
        size_t col = 0;
        stats.setData(p, col++, _players[p]);
        stats.setData(p, col++, (_totalWins[p] + 0.5*_totalDraws[p])/_totalGames[p]);
        stats.setData(p, col++, _totalGames[p]);
        stats.setData(p, col++, _totalWins[p]);
        stats.setData(p, col++, _totalGames[p] - _totalWins[p] - _totalDraws[p]);
        stats.setData(p, col++, _totalDraws[p]);
        stats.setData(p, col++, _totalTurns[p]);
        stats.setData(p, col++, (double)_totalTurns[p] / _totalGames[p]);
        stats.setData(p, col++, (double)_totalTimeMS[p] / _totalTurns[p]);
        stats.setData(p, col++, _maxTimeMS[p]);
    }

    HTMLTable turnTable("Bot vs. Bot Avg Turns Per Game");
    HTMLTable tableWinPerc("Bot vs. Bot Score Table (row score vs. column)");
    std::vector<std::string> header = {""};
    header.insert(header.end(), _players.begin(), _players.end());
    header.push_back("Total");
    turnTable.setHeader(header);
    tableWinPerc.setHeader(header);
    
    std::vector<size_t> colWidth(header.size(), 120);
    turnTable.setColWidth(colWidth);
    tableWinPerc.setColWidth(colWidth);

    for (size_t r(0); r < _players.size(); ++r)
    {
        size_t col = 0;
        turnTable.setData(r, col, _players[r]);
        tableWinPerc.setData(r, col, _players[r]);
        col++;

        for (size_t p(0); p < _players.size(); ++p)
        {
            if (r == p)
            {
                turnTable.setData(r, col, "-");
                tableWinPerc.setData(r, col, "-");
            }
            else
            {
                turnTable.setData(r, col, _numGames[r][p] == 0 ? 0 : (double)_turns[r][p] / _numGames[r][p]);
                tableWinPerc.setData(r, col, _numGames[r][p] == 0 ? 0 : ((double)_wins[r][p] + 0.5*_draws[r][p]) / _numGames[r][p]);
            }

            col++;
        }

        turnTable.setData(r, col, _totalTurns[r]);
        tableWinPerc.setData(r, col, _totalGames[r] == 0 ? 0 : ((double)_totalWins[r] + 0.5*_totalDraws[r]) / _totalGames[r]);
        col++;
    }

    stats.appendHTMLTableToFile(filename, "statsTable");
    tableWinPerc.appendHTMLTableToFile(filename, "winPercentageTable");
    turnTable.appendHTMLTableToFile(filename, "totalScoreTable");
}

void Tournament::printResults() const
{
    std::stringstream ss;
  
    size_t colWidth = 10;
    for (size_t i(0); i < _players.size(); ++i)
    {
        colWidth = std::max(colWidth, _players[i].length() + 2);
    }

    ss << std::endl << std::endl;

    std::stringstream header;
    for (size_t i(0); i < _players.size(); ++i)
    {
        while (header.str().length() < (i+1)*colWidth) header << " ";
        header << _players[i];
    }

    header << "  TotalScore";

    std::cout << header.str() << std::endl;
    ss << header.str() << std::endl;

    for (size_t i(0); i < _players.size(); ++i)
    {
        std::stringstream line;
        line << _players[i]; while (line.str().length() < colWidth) line << " ";

        for (size_t j(0); j < _players.size(); ++j)
        {
            if (_playerGroups[i] != _playerGroups[j])
            {
                line << _wins[i][j] + (0.5*_draws[i][j]) ;
            }
            else
            {
                line << "-";
            }

            while (line.str().length() < colWidth + (j+1)*colWidth) line << " ";
        }

        line << _totalWins[i] + (0.5*_totalDraws[i]);
        line << std::endl;
        ss << line.str();
        std::cout << line.str();
    }
}

int Tournament::getPlayerIndex(const std::string & playerName) const
{
    for (size_t i(0); i < _players.size(); ++i)
    {
        if (_players[i].compare(playerName) == 0)
        {
            return i;
        }
    }

    return -1;
}


std::string Tournament::getTimeStringFromMS(const size_t ms)
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