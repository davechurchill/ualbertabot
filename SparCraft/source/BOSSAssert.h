#pragma once

#include "Common.h"
#include <cstdarg>
#include "BOSSException.h"

#include <ctime>

namespace BOSS
{

class GameState;
namespace Assert
{
    const std::string currentDateTime();
    void ReportFailure(const GameState * state, const char * condition, const char * file, int line, const char * msg, ...);
}
}

#define BOSS_ASSERT_ENABLE

#ifdef BOSS_ASSERT_ENABLE

    #define BOSS_ASSERT(cond, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                BOSS::Assert::ReportFailure(nullptr, #cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__); \
            } \
        } while(0)

    #define BOSS_ASSERT_STATE(cond, state, filename, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                BOSS::Assert::ReportFailure(&state, #cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__); \
            } \
        } while(0)

#else
    #define SPARCRAFT_ASSERT(cond, msg, ...) 
    #define SPARCRAFT_ASSERT_STATE(cond, state, filename, msg, ...) 
#endif
