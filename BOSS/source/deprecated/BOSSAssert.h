#pragma once

#include "Common.h"
#include <cstdio>
#include <cstdarg>
#include "BOSSLogger.h"
#include <sstream>
#include <stdarg.h>

#include <ctime>
#include <iomanip>

extern char BOSS_LOGFILE[100];

#ifdef _MSC_VER
    #define BOSS_BREAK
#else
    #define BOSS_BREAK exit(-1);
#endif

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
        class BOSSException : public std::exception
        {
            std::string s;

        public :

            BOSSException(std::string ss) : s(ss) {}
            ~BOSSException() throw () {} 
            const char* what() const throw() { return s.c_str(); }
        }; 

        void ShutDown();

        extern std::string lastErrorMessage;

        const std::string currentDateTime();

        void ReportFailure(const char * condition, const char * file, int line, const char * msg, ...);
    }
}
