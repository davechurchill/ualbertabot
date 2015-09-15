#include "Logger.h"
#include "UABAssert.h"
#include <stdarg.h>
#include <cstdio>
#include <sstream>

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


std::string FileUtils::ReadFile(const std::string & filename)
{
    std::stringstream ss;

    FILE *file = fopen ( filename.c_str(), "r" );
    if ( file != NULL )
    {
        char line [ 4096 ]; /* or other suitable maximum line size */
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
        {
            ss << line;
        }
        fclose ( file );
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Could not open file: %s", filename.c_str());
    }

    return ss.str();
}