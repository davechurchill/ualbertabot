#include "HLManager.h"
#include "base/ProductionManager.h"

using namespace UAlbertaBot;

HLManager::HLManager()
{
	BWAPI::Broodwar->printf("High Level Manager Instantiated");
	Logger::LogAppendToFile(UAB_LOGFILE, "State size: %d", sizeof(HLState));
}


HLManager::~HLManager()
{
}

HLManager &	HLManager::Instance()
{
	static HLManager instance;
	return instance;
}

void HLManager::update()
{
	//static bool firstRun = true;
	//static int frame = 0;

	//if ((firstRun && !ProductionManager::Instance().runningOpeningBook())
	//	||(!firstRun && (++frame%2000==0)))
	//{
	//	firstRun = false;

		static int cumulativeSearchTime = 0;
		if (cumulativeSearchTime>39000)
		{
			BWAPI::Broodwar->printf("Too much time searching, skipping HL search");
			Logger::LogAppendToFile(UAB_LOGFILE, "\nToo much time searching, skipping HL search");
			cumulativeSearchTime+= _search.search(0, 0, 0);

			return;
		}
		else if (cumulativeSearchTime > 36000)
		{
			BWAPI::Broodwar->printf("Too much time searching, short HL search: %d[ms]", 42000 - cumulativeSearchTime);
			Logger::LogAppendToFile(UAB_LOGFILE, "\nToo much time searching, short HL search: %d[ms]", 42000 - cumulativeSearchTime);
			cumulativeSearchTime+=_search.search(42000 - cumulativeSearchTime, 10000, 40);
		}
		else
		{
			cumulativeSearchTime+=_search.search(6000, 10000, 40);
		}

		auto move = _search.getBestMove();
		StrategyManager::Instance().setCurrentStrategy(move.getStrategy(),move.getChoices());
		BWAPI::Broodwar->printf("Setting move %s\n", move.toString().c_str());
		Logger::LogAppendToFile(UAB_LOGFILE, "Setting move %s\n", move.toString().c_str());

	//	ProductionManager::Instance().purgeQueue();
	//}

}