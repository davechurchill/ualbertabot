#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace BOSS
{
namespace Logger 
{
    void LogAppendToFile(const std::string & logFile, const std::string & msg);
    void LogOverwriteToFile(const std::string & logFile, const std::string & msg);
};
}