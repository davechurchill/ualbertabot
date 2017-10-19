#pragma once

#include "Common.h"
#include "GameState.h"
#include "Move.h"
#include <memory>

namespace SparCraft
{
 
class MoveIterator;

typedef std::shared_ptr<MoveIterator> MoveIteratorPtr; 

class MoveIterator 
{

protected:

    size_t                  _playerID;
    bool                    _hasMoreMoves;
    bool                    _firstMove;

    GameState               _state;
    std::string             _name;
    std::string             _description;

public:
    
    virtual bool                hasMoreMoves() const;
    virtual bool                generateNextChild(GameState & child, Move & movePerformed);
    virtual void                reset();
    virtual void                print();
    virtual void                setState(const GameState & state);
    virtual void                getRandomMove(const GameState & state, Move & move);
    virtual size_t              getPlayerID() const;
    virtual MoveIteratorPtr     clone();
    
    virtual const std::string & getDescription();
    virtual const std::string & getName();
    virtual const GameState &   getState() const;
    virtual GameState &         getState();
};


}