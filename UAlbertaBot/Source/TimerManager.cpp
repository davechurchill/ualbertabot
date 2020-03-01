#include "TimerManager.h"
#include "Config.h"

using namespace UAlbertaBot;

TimerManager::TimerManager()
    : m_timers(std::vector<BOSS::Timer>(NumTypes))
    , m_barWidth(40)
{
    m_timerNames.push_back("Total");
    m_timerNames.push_back("Worker");
    m_timerNames.push_back("Production");
    m_timerNames.push_back("Building");
    m_timerNames.push_back("Combat");
    m_timerNames.push_back("Scout");
    m_timerNames.push_back("UnitInfo");
    m_timerNames.push_back("MapGrid");
    m_timerNames.push_back("MapTools");
    m_timerNames.push_back("Search");
}

void TimerManager::startTimer(const TimerManager::Type t)
{
    m_timers[t].start();
}

void TimerManager::stopTimer(const TimerManager::Type t)
{
    m_timers[t].stop();
}

double TimerManager::getTotalElapsed()
{
    return m_timers[0].getElapsedTimeInMilliSec();
}

void TimerManager::displayTimers(int x, int y)
{
    if (!Config::Debug::DrawModuleTimers)
    {
        return;
    }

    BWAPI::Broodwar->drawBoxScreen(x-5, y-5, x+110+m_barWidth, y+5+(10*m_timers.size()), BWAPI::Colors::Black, true);

    int yskip = 0;
    double total = m_timers[0].getElapsedTimeInMilliSec();
    for (size_t i(0); i<m_timers.size(); ++i)
    {
        double elapsed = m_timers[i].getElapsedTimeInMilliSec();
        if (elapsed > 55)
        {
            BWAPI::Broodwar->printf("Timer Debug: %s %lf", m_timerNames[i].c_str(), elapsed);
        }

        int width = (int)((elapsed == 0) ? 0 : (m_barWidth * (elapsed / total)));

        BWAPI::Broodwar->drawTextScreen(x, y+yskip-3, "\x04 %s", m_timerNames[i].c_str());
        BWAPI::Broodwar->drawBoxScreen(x+60, y+yskip, x+60+width+1, y+yskip+8, BWAPI::Colors::White);
        BWAPI::Broodwar->drawTextScreen(x+70+m_barWidth, y+yskip-3, "%.4lf", elapsed);
        yskip += 10;
    }
}