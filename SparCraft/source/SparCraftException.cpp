#include "SparCraftException.h"

using namespace SparCraft;

SparCraftException::SparCraftException()
    : _s("Default")
{
}

SparCraftException::SparCraftException(std::string ss) 
    : _s(ss) 
{
}

SparCraftException::~SparCraftException() throw () 
{
} 

const char* SparCraftException::what() const throw() 
{ 
    return _s.c_str(); 
}
