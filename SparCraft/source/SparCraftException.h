#pragma once

#include "Common.h"
#include "GameState.h"

namespace SparCraft
{
class SparCraftException : public std::exception
{
    std::string _s;
    GameState _state;
    bool _hasState;

public :

    SparCraftException(std::string ss, const GameState * state);
    SparCraftException(std::string ss);

    ~SparCraftException() throw ();
    const char* what() const throw();
    bool hasState() const;
    const GameState & getState() const;
}; 
}
