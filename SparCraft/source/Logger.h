#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace SparCraft
{

class Logger 
{
    size_t totalCharsLogged;

	Logger();

public:

	static Logger &	Instance();
	void log(const std::string & logFile, std::string & msg);
	void clearLogFile(const std::string & logFile);
};
}