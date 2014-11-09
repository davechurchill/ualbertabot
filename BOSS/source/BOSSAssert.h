#pragma once

#include "Common.h"
#include <cstdio>
#include <cstdarg>
#include "BOSSLogger.h"
#include <sstream>
#include <stdarg.h>

#include <ctime>
#include <iomanip>

#define BOSS_LOGFILE "BOSS_error_log.txt"

#define BOSS_BREAK __debugbreak();

#define BOSS_ASSERT_ALL

#ifdef BOSS_ASSERT_ALL
    #define BOSS_ASSERT(cond, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                BOSS::Assert::ReportFailure(#cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__); \
                BOSS_BREAK \
            } \
        } while(0)
#else
    #define BOSS_ASSERT(cond, msg, ...) 
#endif

namespace BOSS
{
    namespace Assert
    {
        void ShutDown();

        extern std::string lastErrorMessage;

        const std::string currentDateTime();

        void ReportFailure(const char * condition, const char * file, int line, const char * msg, ...);
    }
}
