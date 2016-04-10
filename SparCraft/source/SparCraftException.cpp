#include "SparCraftException.h"

using namespace SparCraft;

SparCraftException::SparCraftException(std::string ss) 
    : _s(ss) 
    , _hasState(false)
{
}

SparCraftException::SparCraftException(std::string ss, const GameState * state) 
    : _s(ss) 
    , _hasState(false)
{
    if (state != nullptr)
    {
        _state = *state;
        _hasState = true;
    }
}

SparCraftException::~SparCraftException() throw () 
{
} 

const char* SparCraftException::what() const throw() 
{ 
    return _s.c_str(); 
}

bool SparCraftException::hasState() const 
{ 
    return _hasState; 
}

const GameState & SparCraftException::getState() const 
{ 
    return _state; 
}