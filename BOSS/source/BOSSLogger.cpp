#include "BOSSLogger.h"

namespace BOSS
{
namespace Logger
{
    void LogAppendToFile(const std::string & logFile, const std::string & msg)
    {
        std::ofstream logStream;
        logStream.open(logFile.c_str(), std::ofstream::app);
        logStream << msg;
        logStream.flush();
        logStream.close();
    }

    void LogOverwriteToFile(const std::string & logFile, const std::string & msg)
    {
        std::ofstream logStream(logFile.c_str());
        logStream << msg;
        logStream.flush();
        logStream.close();
    }
}
}