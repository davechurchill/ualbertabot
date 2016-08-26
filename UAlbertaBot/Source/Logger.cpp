#include "Logger.h"
#include "UABAssert.h"
#include <stdarg.h>
#include <cstdio>
#include <sstream>
#include "BWAPI.h"

using namespace UAlbertaBot;

void Logger::LogAppendToFile(const std::string & logFile, const std::string & msg)
{
    std::ofstream logStream;
    logStream.open(logFile.c_str(), std::ofstream::app);
    logStream << msg;
    logStream.flush();
    logStream.close();
}

void Logger::LogAppendToFile(const std::string & logFile, const char *fmt, ...)
{
	va_list arg;
		
	va_start(arg, fmt);
	//vfprintf(log_file, fmt, arg);
	char buff[256];
	vsnprintf_s(buff, 256, fmt, arg);
	va_end(arg);
		
	std::ofstream logStream;
	logStream.open(logFile.c_str(), std::ofstream::app);
	logStream << buff;
	logStream.flush();
	logStream.close();
}

void Logger::LogOverwriteToFile(const std::string & logFile, const std::string & msg)
{
    std::ofstream logStream(logFile.c_str());
    logStream << msg;
    logStream.flush();
    logStream.close();
}
