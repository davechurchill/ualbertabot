#pragma once

#include "Common.h"
#include "../../BOSS/source/Timer.hpp"

namespace UAlbertaBot
{

class TimerManager
{
    std::vector<BOSS::Timer> m_timers;
    std::vector<std::string> m_timerNames;
    int                      m_barWidth = 0;

public:

    enum Type { All, Worker, Production, Building, Combat, Scout, InformationManager, MapGrid, MapTools, Search, NumTypes };

    TimerManager();

    void startTimer(const TimerManager::Type t);
    void stopTimer(const TimerManager::Type t);
    void displayTimers(int x, int y);
    double getTotalElapsed();
};

}