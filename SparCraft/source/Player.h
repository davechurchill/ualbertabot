#pragma once

#include "Common.h"
#include "ActionGenerators.h"
#include "AITools.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include "Move.h"
#include "Timer.h"
#include <memory>

namespace SparCraft
{

class Player;
typedef std::shared_ptr<Player> PlayerPtr; 

class Player 
{
protected:

    size_t      _playerID;
    Timer       _timer;
    std::string _name;
    std::string _description;
    std::string _resultString;

    double      _previousResultTime;
    double      _totalResultTime;
    double      _maxResultTime;
    size_t      _calls;

public:

    Player();
    virtual void		    getMove(const GameState & state, Move & move);
    const size_t            ID();
    virtual void            setName(const std::string & name);
    virtual PlayerPtr       clone();
    virtual void            startTimer();
    virtual void            stopTimer();

    virtual const std::string & getName();
    virtual const std::string & getDescription();
    virtual const std::string & getResultString();
};


}