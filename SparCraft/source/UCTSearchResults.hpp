#pragma once

#include <vector>
#include "UnitAction.hpp"

namespace SparCraft
{
class UCTSearchResults
{

public:

	unsigned long long          nodesExpanded;	// number of nodes expanded in the search
	double                      timeElapsed;	// time elapsed in milliseconds

    int                         traversals;
    int                         traverseCalls;
    int                         nodesVisited;
    int                         totalVisits;
    int                         nodesCreated;

    std::vector<UnitAction>     bestMoves;
	ScoreType                   abValue;
	
    std::vector<std::vector<std::string> > _desc;    // 2-column description vector

	UCTSearchResults() 
		: nodesExpanded         (0)
		, timeElapsed           (0)
        , traversals            (0)
        , traverseCalls         (0)
        , nodesVisited          (0)
        , totalVisits           (0)
        , nodesCreated          (0)
		, abValue               (0)
	{
	}

    std::vector<std::vector<std::string> > & getDescription()
    {
        _desc.clear();
        _desc.push_back(std::vector<std::string>());
        _desc.push_back(std::vector<std::string>());

        std::stringstream ss;

        _desc[0].push_back("Traversals: ");
        _desc[0].push_back("Nodes Visited: ");
        _desc[0].push_back("Total Visits: ");
        _desc[0].push_back("Nodes Created: ");

        ss << traversals;       _desc[1].push_back(ss.str()); ss.str(std::string());
        ss << nodesVisited;     _desc[1].push_back(ss.str()); ss.str(std::string());
        ss << totalVisits;      _desc[1].push_back(ss.str()); ss.str(std::string());
        ss << nodesCreated;     _desc[1].push_back(ss.str()); ss.str(std::string());
        
        return _desc;
    }
};
}