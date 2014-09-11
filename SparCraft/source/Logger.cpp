#include "Logger.h"

using namespace SparCraft;

Logger::Logger() 
    : totalCharsLogged(0)
{
	
}

Logger & Logger::Instance() 
{
	static Logger instance;
	return instance;
}

void Logger::clearLogFile(const std::string & logFile)
{

}

void Logger::log(const std::string & logFile, std::string & msg)
{
    std::ofstream logStream;
	logStream.open(logFile.c_str(), std::ofstream::app);
	logStream << msg;
    logStream.flush();
    logStream.close();
}
