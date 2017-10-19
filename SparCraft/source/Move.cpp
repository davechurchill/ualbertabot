#include "Move.h"

using namespace SparCraft;

Move::Move()
{

}

bool Move::operator == (const Move & rhs) const
{
    for (size_t a(0); a<size(); ++a)
    {
        if (_actions[a] != rhs._actions[a])
        {
            return false;
        }
    }

    return true;
}

const Action & Move::operator [](const size_t & index) const
{
    return _actions[index];
}

Action & Move::operator [](const size_t & index)
{
    return _actions[index];
}

void Move::set(const Move & move)
{
    _actions = move._actions;
}

void Move::addMove(const Move & move)
{
    for (size_t m(0); m<move.size(); ++m)
    {
        addAction(move.getAction(m));
    }
}

void Move::addAction(const Action & action)
{
    SPARCRAFT_ASSERT((_actions.size() == 0) || (action.getPlayerID() == _actions[0].getPlayerID()), "All Actions in a Move must have same PlayerID");

    _actions.push_back(action);
}

const size_t Move::size() const
{
    return _actions.size();
}

const Action & Move::getAction(const size_t & index) const
{
    return _actions[index];
}

void Move::clear()
{
    _actions.clear();
}

void Move::popAction()
{
    _actions.pop_back();
}