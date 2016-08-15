#pragma once

#include "Common.h"
#include "ActionGenerators.h"
#include "AITools.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include "Move.h"
#include <memory>

namespace SparCraft
{

class Player;
typedef std::shared_ptr<Player> PlayerPtr; 

class Player 
{
protected:
    size_t                  _playerID;
    std::string             _description;
public:
    virtual void		    getMove(const GameState & state, Move & move);
    const size_t            ID();
    virtual std::string     getDescription();
    virtual void            setDescription(const std::string & desc);

    virtual PlayerPtr       clone();
};


}