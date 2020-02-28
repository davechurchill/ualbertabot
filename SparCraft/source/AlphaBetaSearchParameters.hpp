#pragma once

#include "Common.h"

namespace SparCraft
{
    class AlphaBetaSearchParameters;
}

class SparCraft::AlphaBetaSearchParameters
{											        // DEFAULT				DESCRIPTION
    size_t          _searchMethod;                  // ID-AB                The Method to use for AB Search
    size_t		    _maxPlayer;					    // Player_One			The player who will make maximizing moves
    size_t          _maxDepth;                      // Max_Depth            Maximum depth of AB search to allow

	size_t		    _timeLimit;					    // 0					Search time limit. 0 means no time limit
    size_t          _maxChildren;                   // 10                   Max children at each node
    size_t          _moveOrdering;                  // ScriptFirst          Move ordering method for child generation
	size_t		    _evalMethod;				    // LTD				Evaluation function type
    size_t          _simScripts[2];                 // NOKDPS               Policy to use for playouts
	size_t		    _playerToMoveMethod;		    // Alternate			The player to move policy
	size_t		    _playerModel[2];                // None                 Player model to use for each player

    std::string     _graphVizFilename;              // ""                   File name to output graph viz file

    std::vector<size_t> _orderedMoveScripts;

    std::vector<std::vector<std::string> > _desc;    // 2-column description vector

public:

	// default constructor
	AlphaBetaSearchParameters() 
        : _searchMethod         (SearchMethods::IDAlphaBeta)
        , _maxPlayer            (Players::Player_One)
        , _maxDepth             (Constants::Max_Search_Depth)
	    , _timeLimit            (0)
        , _maxChildren          (10)
        , _moveOrdering         (MoveOrderMethod::ScriptFirst)
        , _evalMethod           (SparCraft::EvaluationMethods::Playout)
	    , _playerToMoveMethod   (SparCraft::PlayerToMove::Alternate)
    {
	    setPlayerModel(Players::Player_One, PlayerModels::None);
	    setPlayerModel(Players::Player_Two, PlayerModels::None);
        setSimScripts(PlayerModels::NOKDPS, PlayerModels::NOKDPS);
    }

    const size_t & searchMethod()							    const   { return _searchMethod; }
    const size_t & maxPlayer()							        const   { return _maxPlayer; }
    const size_t & maxDepth()							        const   { return _maxDepth; }
    const size_t & timeLimit()							        const   { return _timeLimit; }
    const size_t & maxChildren()                                const   { return _maxChildren; }
    const size_t & moveOrderingMethod()                         const   { return _moveOrdering; }
    const size_t & evalMethod()						            const   { return _evalMethod; }
    const size_t & simScript(const size_t & player)             const   { return _simScripts[player]; }
    const size_t & playerToMoveMethod()				            const   { return _playerToMoveMethod; }
    const size_t & playerModel(const size_t & player)	        const   { return _playerModel[player]; }
    const std::string & graphVizFilename()                      const   { return _graphVizFilename; }
    const std::vector<size_t> & getOrderedMoveScripts()         const   { return _orderedMoveScripts; }
	
    void setSearchMethod(const size_t & method)                         { _searchMethod = method; }
    void setMaxPlayer(const size_t & player)					        { _maxPlayer = player; }
    void setMaxDepth(const size_t & depth)                              { _maxDepth = depth; }
    
    void setTimeLimit(const size_t & timeLimit)					        { _timeLimit = timeLimit; }
    void setMaxChildren(const size_t & children)                        { _maxChildren = children; }
    void setMoveOrderingMethod(const size_t & method)                   { _moveOrdering = method; }
    void setEvalMethod(const size_t & eval)						        { _evalMethod = eval; }
    void setSimScripts(const size_t & p1, const size_t & p2)		    { _simScripts[0] = p1; _simScripts[1] = p2; }
    void setPlayerToMoveMethod(const size_t & method)				    { _playerToMoveMethod = method; }
    void setGraphVizFilename(const std::string & filename)              { _graphVizFilename = filename; }
    void addOrderedMoveScript(const size_t & script)                    { _orderedMoveScripts.push_back(script); }
    void setPlayerModel(const size_t & player, const size_t & model)	{ _playerModel[player] = model; }	

    std::vector<std::vector<std::string> > & getDescription()
    {
        if (_desc.size() == 0)
        {
            _desc.push_back(std::vector<std::string>());
            _desc.push_back(std::vector<std::string>());

            std::stringstream ss;

            _desc[0].push_back("Player Type:");
            _desc[0].push_back("Time Limit:");
            _desc[0].push_back("Max Children:");
            _desc[0].push_back("Move Ordering:");
            _desc[0].push_back("Player To Move:");
            _desc[0].push_back("Opponent Model:");

            ss << "AlphaBeta";                                              _desc[1].push_back(ss.str()); ss.str(std::string());
            ss << timeLimit() << "ms";                                      _desc[1].push_back(ss.str()); ss.str(std::string());
            ss << maxChildren();                                            _desc[1].push_back(ss.str()); ss.str(std::string());
            ss << MoveOrderMethod::getName(moveOrderingMethod());             _desc[1].push_back(ss.str()); ss.str(std::string());
            ss << PlayerToMove::getName(playerToMoveMethod());                _desc[1].push_back(ss.str()); ss.str(std::string());
            ss << PlayerModels::getName(playerModel((maxPlayer()+1)%2));   _desc[1].push_back(ss.str()); ss.str(std::string());
        }
        
        return _desc;
    }
};
