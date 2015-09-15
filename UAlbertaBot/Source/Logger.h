#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace UAlbertaBot
{
namespace Logger 
{
    void LogAppendToFile(const std::string & logFile, const std::string & msg);
	void LogAppendToFile(const std::string & logFile, const char *fmt, ...);
    void LogOverwriteToFile(const std::string & logFile, const std::string & msg);
};

namespace FileUtils
{
    std::string ReadFile(const std::string & filename);
}
}