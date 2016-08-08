#include "BOSSAssert.h"
#include "BOSSException.h"

using namespace BOSS;

namespace BOSS
{
namespace Assert
{
    const std::string currentDateTime() 
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d_%X", &tstruct);

        for (size_t i(0); i < strlen(buf); ++i)
        {
            if (buf[i] == ':')
            {
                buf[i] = '-';
            }
        }

        return buf;
    }

    void ReportFailure(const GameState * state, const char * condition, const char * file, int line, const char * msg, ...)
    {
        std::cerr << "Assertion thrown!\n";

        char messageBuffer[4096] = "";
        if (msg != NULL)
        {
            va_list args;
            va_start(args, msg);
            vsprintf(messageBuffer, msg, args);
            va_end(args);
        }

        std::stringstream ss;
        ss                                      << std::endl;
        ss << "!Assert:   " << condition        << std::endl;
        ss << "File:      " << file             << std::endl;
        ss << "Message:   " << messageBuffer    << std::endl;
        ss << "Line:      " << line             << std::endl;
        
        #if !defined(EMSCRIPTEN)
            std::cerr << ss.str();  
            throw BOSSException(ss.str());
        #else
            printf("C++ AI: AI Exception Thrown:\n %s\n", ss.str().c_str());
            throw BOSSException(ss.str());
        #endif
    }
}
}

