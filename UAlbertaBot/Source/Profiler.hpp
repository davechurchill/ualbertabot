#pragma once

#include <chrono>
#include <fstream>
#include <mutex>
#include <map>
#include <algorithm>

//#define PROFILING 1
#ifdef PROFILING
    #define PROFILE_SCOPE(name) \
        ProfileTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() \
        PROFILE_SCOPE(__FUNCTION__)
#else
    #define PROFILE_FUNCTION()
    #define PROFILE_SCOPE(name)
#endif

namespace UAlbertaBot
{


struct ProfileResult
{
    std::string name = "Default";
    long long start  = 0;
    long long end    = 0;
    size_t threadID  = 0;
};

class Profiler
{
    std::string     m_outputFile    = "results.json";
    size_t          m_profileCount  = 0;
    std::ofstream   m_outputStream;
    std::mutex      m_lock;

    Profiler()
    { 
        m_outputStream = std::ofstream(m_outputFile);
        writeHeader();
    }

    void writeHeader() { m_outputStream << "{\"otherData\": {},\"traceEvents\":["; }
    void writeFooter() { m_outputStream << "]}"; }

public:

    static Profiler& Instance()
    {
        static Profiler instance;
        return instance;
    }

    ~Profiler()
    {
        writeFooter();
    }
    
    void writeProfile(const ProfileResult& result)
    {
        std::lock_guard<std::mutex> lock(m_lock);

        if (m_profileCount++ > 0) { m_outputStream << ","; }

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << "\n{";
        m_outputStream << "\"cat\":\"function\",";
        m_outputStream << "\"dur\":" << (result.end - result.start) << ',';
        m_outputStream << "\"name\":\"" << name << "\",";
        m_outputStream << "\"ph\":\"X\",";
        m_outputStream << "\"pid\":0,";
        m_outputStream << "\"tid\":" << result.threadID << ",";
        m_outputStream << "\"ts\":" << result.start;
        m_outputStream << "}";
    }
};

class ProfileTimer
{
    ProfileResult m_result;
    bool m_stopped = false;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;

public:

    ProfileTimer(const std::string & name)
    {
        m_result.name = name;
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~ProfileTimer()
    {
        stop();
    }

    void stop()
    {
        if (m_stopped) { return; }

        auto endTimepoint = std::chrono::high_resolution_clock::now();

        m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        m_result.end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        m_result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Profiler::Instance().writeProfile(m_result);

        m_stopped = true;
    }
};
}