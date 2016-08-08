#include "BOSSException.h"

using namespace BOSS;

BOSSException::BOSSException(std::string ss) 
    : _s(ss) 
    , _hasState(false)
{
}

//BOSSException::BOSSException(std::string ss, const GameState * state) 
//    : _s(ss) 
//    , _hasState(false)
//{
//    if (state != nullptr)
//    {
//        _state = *state;
//        _hasState = true;
//    }
//}

BOSSException::~BOSSException() throw () 
{
} 

const char* BOSSException::what() const throw() 
{ 
    return _s.c_str(); 
}

bool BOSSException::hasState() const 
{ 
    return _hasState; 
}

//const GameState & BOSSException::getState() const 
//{ 
//    return _state; 
//}