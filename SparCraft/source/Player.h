#pragma once

#include "Common.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include <memory>

namespace SparCraft
{

class Player;
typedef std::shared_ptr<Player> PlayerPtr; 

class Player 
{
protected:
    IDType                  _playerID;
    std::string             _description;
public:
    virtual void		    getMoves(const GameState & state, std::vector<Action> & moveVec);
    const IDType            ID();
    virtual std::string     getDescription();
    virtual void            setDescription(const std::string & desc);

    virtual PlayerPtr       clone();
};


}