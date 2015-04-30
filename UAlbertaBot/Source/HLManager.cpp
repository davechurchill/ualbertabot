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

void HLManager::update(
	std::set<BWAPI::UnitInterface*> combatUnits,
	std::set<BWAPI::UnitInterface*> scoutUnits,
	std::set<BWAPI::UnitInterface*> workerUnits)
{

	static bool firstRun = true;
	static int frame = 0;
	if ((firstRun && !ProductionManager::Instance().runningOpeningBook())
		||(!firstRun && (++frame%1500==0)))
	{
		firstRun = false;
		static int searchNumber = 0;
		if (++searchNumber > 7)
		{
			BWAPI::Broodwar->printf("Too much time searching, skipping HL search");
			return;
		}
		_search.search(6000, 10000); 
		auto move = _search.getBestMove();
		StrategyManager::Instance().setCurrentStrategy(move.getStrategy(),move.getChoices());
		BWAPI::Broodwar->printf("Setting move %s\n", move.toString().c_str());
		Logger::LogAppendToFile(UAB_LOGFILE, "Setting move %s\n", move.toString().c_str());

		ProductionManager::Instance().purgeQueue();
	}

}