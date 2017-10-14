#pragma once

#include "Common.h"
#include "Action.h"

namespace SparCraft
{
 
class Move
{
    std::vector<Action>     _actions;
    
public:

    Move();

    bool operator == (const Move & rhs) const;
    const Action & operator [](const size_t & index) const;
    Action & operator [](const size_t & index);

    void set(const Move & move);
    void addAction(const Action & action);
    void addMove(const Move & move);
    const size_t size() const;
    const Action & getAction(const size_t & index) const;
    void clear();
    void popAction();
};

}