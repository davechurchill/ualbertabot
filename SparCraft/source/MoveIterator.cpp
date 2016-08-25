#include "MoveIterator.h"

using namespace SparCraft;

bool MoveIterator::hasMoreMoves() const
{
    return _hasMoreMoves;
}

bool MoveIterator::generateNextChild(GameState & child, Move & movePerformed)
{
    SPARCRAFT_ASSERT(false, "Base MoveIterator::getNextState called for some reason");
    return false;
}

void MoveIterator::reset()
{
    SPARCRAFT_ASSERT(false, "Base MoveIterator::reset called for some reason");
}

void MoveIterator::print()
{
    SPARCRAFT_ASSERT(false, "Base MoveIterator::reset called for some reason");
}

void MoveIterator::setState(const GameState & state)
{
    SPARCRAFT_ASSERT(false, "Base MoveIterator::setState called for some reason");
}

void MoveIterator::getRandomMove(const GameState & state, Move & move)
{
    SPARCRAFT_ASSERT(false, "Base MoveIterator::getRandomMove called for some reason");
}

size_t MoveIterator::getPlayerID() const
{
    return _playerID;
}

MoveIteratorPtr MoveIterator::clone()
{
    return MoveIteratorPtr(new MoveIterator(*this));
}

const GameState & MoveIterator::getState() const
{
    return _state;
}

GameState & MoveIterator::getState()
{
    return _state;
}

const std::string & MoveIterator::getDescription()
{ 
    return _description; 
}

const std::string & MoveIterator::getName()
{ 
    return _name; 
}
