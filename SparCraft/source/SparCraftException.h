#pragma once

#include "Common.h"

namespace SparCraft
{
class SparCraftException : public std::exception
{
    std::string _s;

public :

    SparCraftException(std::string ss);

    ~SparCraftException() throw ();
    const char* what() const throw();
}; 
}
