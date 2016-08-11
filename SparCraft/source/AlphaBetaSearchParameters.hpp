#pragma once

#include "Common.h"

namespace SparCraft
{
    class AlphaBetaSearchParameters;
}

class SparCraft::AlphaBetaSearchParameters
{											        // DEFAULT				DESCRIPTION
    PlayerID          _searchMethod;                  // ID-AB                The Method to use for AB Search
    PlayerID		    _maxPlayer;					    // Player_One			The player who will make maximizing moves
    PlayerID          _maxDepth;                      // Max_Depth            Maximum depth of AB search to allow

	size_t		    _timeLimit;					    // 0					Search time limit. 0 means no time limit
    size_t          _maxChildren;                   // 10                   Max children at each node
    PlayerID          _moveOrdering;                  // ScriptFirst          Move ordering method for child generation
	PlayerID		    _evalMethod;				    // LTD				Evaluation function type
    PlayerID          _simScripts[2];                 // NOKDPS               Policy to use for playouts
	PlayerID		    _playerToMoveMethod;		    // Alternate			The player to move policy
	PlayerID		    _playerModel[2];                // None                 Player model to use for each player

    std::string     _graphVizFilename;              // ""                   File name to output graph viz file

    std::vector<PlayerID> _orderedMoveScripts;

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

    const PlayerID & searchMethod()							    const   { return _searchMethod; }
    const PlayerID & maxPlayer()							        const   { return _maxPlayer; }
    const PlayerID & maxDepth()							        const   { return _maxDepth; }
    const size_t & timeLimit()							        const   { return _timeLimit; }
    const size_t & maxChildren()                                const   { return _maxChildren; }
    const PlayerID & moveOrderingMethod()                         const   { return _moveOrdering; }
    const PlayerID & evalMethod()						            const   { return _evalMethod; }
    const PlayerID & simScript(const PlayerID & player)             const   { return _simScripts[player]; }
    const PlayerID & playerToMoveMethod()				            const   { return _playerToMoveMethod; }
    const PlayerID & playerModel(const PlayerID & player)	        const   { return _playerModel[player]; }
    const std::string & graphVizFilename()                      const   { return _graphVizFilename; }
    const std::vector<PlayerID> & getOrderedMoveScripts()         const   { return _orderedMoveScripts; }
	
    void setSearchMethod(const PlayerID & method)                         { _searchMethod = method; }
    void setMaxPlayer(const PlayerID & player)					        { _maxPlayer = player; }
    void setMaxDepth(const PlayerID & depth)                              { _maxDepth = depth; }
    
    void setTimeLimit(const size_t & timeLimit)					        { _timeLimit = timeLimit; }
    void setMaxChildren(const size_t & children)                        { _maxChildren = children; }
    void setMoveOrderingMethod(const size_t & method)                   { _moveOrdering = method; }
    void setEvalMethod(const PlayerID & eval)						        { _evalMethod = eval; }
    void setSimScripts(const PlayerID & p1, const PlayerID & p2)		    { _simScripts[0] = p1; _simScripts[1] = p2; }
    void setPlayerToMoveMethod(const PlayerID & method)				    { _playerToMoveMethod = method; }
    void setGraphVizFilename(const std::string & filename)              { _graphVizFilename = filename; }
    void addOrderedMoveScript(const PlayerID & script)                    { _orderedMoveScripts.push_back(script); }
    void setPlayerModel(const PlayerID & player, const PlayerID & model)	{ _playerModel[player] = model; }	

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
            //ss << MoveOrderMethod::getName(moveOrderingMethod());             _desc[1].push_back(ss.str()); ss.str(std::string());
            //ss << PlayerToMove::getName(playerToMoveMethod());                _desc[1].push_back(ss.str()); ss.str(std::string());
            //ss << PlayerModels::getName(playerModel((maxPlayer()+1)%2));   _desc[1].push_back(ss.str()); ss.str(std::string());
        }
        
        return _desc;
    }
};
