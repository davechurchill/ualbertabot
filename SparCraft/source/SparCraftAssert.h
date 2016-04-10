#pragma once

#include "Common.h"
#include <cstdarg>

#include <ctime>

namespace SparCraft
{

class GameState;
namespace Assert
{
    const std::string currentDateTime();
    void ReportFailure(const GameState * state, const char * condition, const char * file, int line, const char * msg, ...);
}
}

#define SPARCRAFT_ASSERT_ENABLE

#ifdef SPARCRAFT_ASSERT_ENABLE

    #define SPARCRAFT_ASSERT(cond, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                SparCraft::Assert::ReportFailure(nullptr, #cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__); \
            } \
        } while(0)

    #define SPARCRAFT_ASSERT_STATE(cond, state, filename, msg, ...) \
        do \
        { \
            if (!(cond)) \
            { \
                SparCraft::Assert::ReportFailure(&state, #cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__); \
            } \
        } while(0)

#else
    #define SPARCRAFT_ASSERT(cond, msg, ...) 
    #define SPARCRAFT_ASSERT_STATE(cond, state, filename, msg, ...) 
#endif
