#include "TimerManager.h"

using namespace UAlbertaBot;

TimerManager::TimerManager() 
    : _timers(std::vector<BOSS::Timer>(NumTypes))
    , _barWidth(40)
{
	_timerNames.push_back("Total");
	_timerNames.push_back("Worker");
	_timerNames.push_back("Production");
	_timerNames.push_back("Building");
	_timerNames.push_back("Combat");
	_timerNames.push_back("Scout");
	_timerNames.push_back("UnitInfo");
	_timerNames.push_back("MapGrid");
	_timerNames.push_back("MapTools");
	_timerNames.push_back("Search");
}

void TimerManager::startTimer(const TimerManager::Type t)
{
	_timers[t].start();
}

void TimerManager::stopTimer(const TimerManager::Type t)
{
	_timers[t].stop();
}

double TimerManager::getTotalElapsed()
{
	return _timers[0].getElapsedTimeInMilliSec();
}

void TimerManager::displayTimers(int x, int y)
{
    if (!Config::Debug::DrawModuleTimers)
    {
        return;
    }

	BWAPI::Broodwar->drawBoxScreen(x-5, y-5, x+110+_barWidth, y+5+(10*_timers.size()), BWAPI::Colors::Black, true);

	int yskip = 0;
	double total = _timers[0].getElapsedTimeInMilliSec();
	for (size_t i(0); i<_timers.size(); ++i)
	{
		double elapsed = _timers[i].getElapsedTimeInMilliSec();
        if (elapsed > 55)
        {
            BWAPI::Broodwar->printf("Timer Debug: %s %lf", _timerNames[i].c_str(), elapsed);
        }

		int width = (int)((elapsed == 0) ? 0 : (_barWidth * (elapsed / total)));

		BWAPI::Broodwar->drawTextScreen(x, y+yskip-3, "\x04 %s", _timerNames[i].c_str());
		BWAPI::Broodwar->drawBoxScreen(x+60, y+yskip, x+60+width+1, y+yskip+8, BWAPI::Colors::White);
		BWAPI::Broodwar->drawTextScreen(x+70+_barWidth, y+yskip-3, "%.4lf", elapsed);
		yskip += 10;
	}
}