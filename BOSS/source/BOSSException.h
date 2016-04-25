#pragma once

#include "Common.h"

namespace BOSS
{
class BOSSException : public std::exception
{
    std::string _s;
    bool _hasState;

public :

    //BOSSException(std::string ss, const GameState * state);
    BOSSException(std::string ss);

    ~BOSSException() throw ();
    const char* what() const throw();
    bool hasState() const;
    //const GameState & getState() const;
}; 
}
