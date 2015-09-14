#include "Common.h"
#include "StrategyManager.h"

using namespace UAlbertaBot;

std::unordered_map<int, std::unordered_map<int, std::unordered_map<short, short> > > StrategyManager::defaultStrategyChoices;
// constructor
StrategyManager::StrategyManager() 
	: firstAttackSent(false)
	, currentStrategy(0)
	, selfRace(BWAPI::Broodwar->self()->getRace())
	, enemyRace(BWAPI::Broodwar->enemy()->getRace())
{
	addStrategies();
	setStrategy();
}

// get an instance of this
StrategyManager & StrategyManager::Instance() 
{
	static StrategyManager instance;
	return instance;
}

void StrategyManager::addStrategies() 
{
	

	////protossOpeningBook[ProtossZealotRush]	= "0 0 0 0 1 0 0 3 0 0 3 0 1 3 0 4 4 4 4 4 1 0 4 4 4";
 //   protossOpeningBook[ProtossZealotRush]	= "0 0 0 0 1 0 3 3 0 0 4 1 4 4 0 4 4 0 1 4 3 0 1 0 4 0 4 4 4 4 1 0 4 4 4";
	////protossOpeningBook[ProtossZealotRush]	= "0";
	////protossOpeningBook[ProtossDarkTemplar]	= "0 0 0 0 1 3 0 7 5 0 0 12 3 13 0 22 22 22 22 0 1 0";
 //   protossOpeningBook[ProtossDarkTemplar]	=     "0 0 0 0 1 0 3 0 7 0 5 0 12 0 13 3 22 22 1 22 22 0 1 0";
	//protossOpeningBook[ProtossDragoons]		= "0 0 0 0 1 0 0 3 0 7 0 0 5 0 0 3 8 6 1 6 6 0 3 1 0 6 6 6";
	//terranOpeningBook[TerranMarineRush]		= "0 0 0 0 0 1 0 0 3 0 0 3 0 1 0 4 0 0 0 6";
	//zergOpeningBook[ZergZerglingRush]		= "0 0 0 0 0 1 0 0 0 2 3 5 0 0 0 0 0 0 1 6";

	if (selfRace == BWAPI::Races::Protoss)
	{
		results = std::vector<IntPair>(NumProtossStrategies);

		if (enemyRace == BWAPI::Races::Protoss)
		{
			usableStrategies.push_back(ProtossZealotRush);
			usableStrategies.push_back(ProtossDarkTemplar);
			usableStrategies.push_back(ProtossDragoons);
		}
		else if (enemyRace == BWAPI::Races::Terran)
		{
			usableStrategies.push_back(ProtossZealotRush);
			usableStrategies.push_back(ProtossDarkTemplar);
			usableStrategies.push_back(ProtossDragoons);
		}
		else if (enemyRace == BWAPI::Races::Zerg)
		{
			usableStrategies.push_back(ProtossZealotRush);
			usableStrategies.push_back(ProtossDragoons);
		}
		else
		{
			BWAPI::Broodwar->printf("Enemy Race Unknown");
			usableStrategies.push_back(ProtossZealotRush);
			usableStrategies.push_back(ProtossDragoons);
		}
	}
	else if (selfRace == BWAPI::Races::Terran)
	{
		results = std::vector<IntPair>(NumTerranStrategies);
		usableStrategies.push_back(TerranMarineRush);
	}
	else if (selfRace == BWAPI::Races::Zerg)
	{
		results = std::vector<IntPair>(NumZergStrategies);
		usableStrategies.push_back(ZergZerglingRush);
	}

	if (Options::Modules::USING_STRATEGY_IO)
	{
		readResults();
	}
}

void StrategyManager::readResults()
{
	// read in the name of the read and write directories from settings file
	struct stat buf;

	// if the file doesn't exist something is wrong so just set them to default settings
	if (stat(Options::FileIO::FILE_SETTINGS, &buf) == -1)
	{
		readDir = "bwapi-data/testio/read/";
		writeDir = "bwapi-data/testio/write/";
	}
	else
	{
		std::ifstream f_in(Options::FileIO::FILE_SETTINGS);
		getline(f_in, readDir);
		getline(f_in, writeDir);
		f_in.close();
	}

	// the file corresponding to the enemy's previous results
	std::string readFile = readDir + BWAPI::Broodwar->enemy()->getName() + ".txt";

	// if the file doesn't exist, set the results to zeros
	if (stat(readFile.c_str(), &buf) == -1)
	{
		std::fill(results.begin(), results.end(), IntPair(0,0));
	}
	// otherwise read in the results
	else
	{
		std::ifstream f_in(readFile.c_str());
		std::string line;
		getline(f_in, line);
		results[ProtossZealotRush].first = atoi(line.c_str());
		getline(f_in, line);
		results[ProtossZealotRush].second = atoi(line.c_str());
		getline(f_in, line);
		results[ProtossDarkTemplar].first = atoi(line.c_str());
		getline(f_in, line);
		results[ProtossDarkTemplar].second = atoi(line.c_str());
		getline(f_in, line);
		results[ProtossDragoons].first = atoi(line.c_str());
		getline(f_in, line);
		results[ProtossDragoons].second = atoi(line.c_str());
		f_in.close();
	}

	BWAPI::Broodwar->printf("Results (%s): (%d %d) (%d %d) (%d %d)", BWAPI::Broodwar->enemy()->getName().c_str(), 
		results[0].first, results[0].second, results[1].first, results[1].second, results[2].first, results[2].second);
}

void StrategyManager::writeResults()
{
	std::string writeFile = writeDir + BWAPI::Broodwar->enemy()->getName() + ".txt";
	std::ofstream f_out(writeFile.c_str());

	f_out << results[ProtossZealotRush].first   << "\n";
	f_out << results[ProtossZealotRush].second  << "\n";
	f_out << results[ProtossDarkTemplar].first  << "\n";
	f_out << results[ProtossDarkTemplar].second << "\n";
	f_out << results[ProtossDragoons].first     << "\n";
	f_out << results[ProtossDragoons].second    << "\n";

	f_out.close();
}

void StrategyManager::setStrategy()
{
	// if we are using file io to determine strategy, do so
	if (Options::Modules::USING_STRATEGY_IO)
	{
		double bestUCB = -1;
		int bestStrategyIndex = 0;

		// UCB requires us to try everything once before using the formula
		for (size_t strategyIndex(0); strategyIndex<usableStrategies.size(); ++strategyIndex)
		{
			int sum = results[usableStrategies[strategyIndex]].first + results[usableStrategies[strategyIndex]].second;

			if (sum == 0)
			{
				currentStrategy = usableStrategies[strategyIndex];
				return;
			}
		}

		// if we have tried everything once, set the maximizing ucb value
		for (size_t strategyIndex(0); strategyIndex<usableStrategies.size(); ++strategyIndex)
		{
			double ucb = getUCBValue(usableStrategies[strategyIndex]);

			if (ucb > bestUCB)
			{
				bestUCB = ucb;
				bestStrategyIndex = strategyIndex;
			}
		}
		
		currentStrategy = usableStrategies[bestStrategyIndex];
	}
	else if (Options::Modules::USING_HIGH_LEVEL_SEARCH)
	{
		currentStrategy = ProtossHighLevelSearch;
		defaultStrategyChoices[BWAPI::Races::Protoss.getID()][ProtossHighLevelSearch][0] = 0;
		defaultStrategyChoices[BWAPI::Races::Protoss.getID()][ProtossHighLevelSearch][1] = 0;
		defaultStrategyChoices[BWAPI::Races::Protoss.getID()][ProtossHighLevelSearch][2] = 0;
		strategyChoices[BWAPI::Races::Protoss.getID()][ProtossHighLevelSearch];//to create the empty map
	}
	else
	{
		// otherwise return a random strategy

        std::string enemyName(BWAPI::Broodwar->enemy()->getName());
        
        if (enemyName.compare("Skynet") == 0)
        {
            currentStrategy = ProtossDarkTemplar;
        }
        else
        {
            currentStrategy = ProtossZealotRush;
        }
	}

}

void StrategyManager::onEnd(const bool isWinner)
{
	// write the win/loss data to file if we're using IO
	if (Options::Modules::USING_STRATEGY_IO)
	{
		// if the game ended before the tournament time limit
		if (BWAPI::Broodwar->getFrameCount() < Options::Tournament::GAME_END_FRAME)
		{
			if (isWinner)
			{
				results[getCurrentStrategy()].first = results[getCurrentStrategy()].first + 1;
			}
			else
			{
				results[getCurrentStrategy()].second = results[getCurrentStrategy()].second + 1;
			}
		}
		// otherwise game timed out so use in-game score
		else
		{
			if (getScore(BWAPI::Broodwar->self()) > getScore(BWAPI::Broodwar->enemy()))
			{
				results[getCurrentStrategy()].first = results[getCurrentStrategy()].first + 1;
			}
			else
			{
				results[getCurrentStrategy()].second = results[getCurrentStrategy()].second + 1;
			}
		}
		
		writeResults();
	}
}

const double StrategyManager::getUCBValue(const size_t & strategy) const
{
	double totalTrials(0);
	for (size_t s(0); s<usableStrategies.size(); ++s)
	{
		totalTrials += results[usableStrategies[s]].first + results[usableStrategies[s]].second;
	}

	double C		= 0.7;
	double wins		= results[strategy].first;
	double trials	= results[strategy].first + results[strategy].second;

	double ucb = (wins / trials) + C * sqrt(std::log(totalTrials) / trials);

	return ucb;
}

const int StrategyManager::getScore(BWAPI::PlayerInterface * player) const
{
	return player->getBuildingScore() + player->getKillScore() + player->getRazingScore() + player->getUnitScore();
}


std::vector<MetaType>	StrategyManager::getOpeningBookBuildOrder(int strategy, BWAPI::Race race)
{
	if (race == BWAPI::Races::Protoss)
	{
		if (strategy == ProtossZealotRush)
		{
			return OpeningBuildOrders::ProtossZealotRush();
		}
		else if (strategy == ProtossDragoons)
		{
			return OpeningBuildOrders::ProtossDragoonRush();
		}
		else if (strategy == ProtossDarkTemplar)
		{
			return OpeningBuildOrders::ProtossDarkTemplarRush();
		}
		else if (strategy == ProtossHighLevelSearch)
		{
			return OpeningBuildOrders::ProtossHighLevelSearch();
		}

	}
	else if (race == BWAPI::Races::Terran)
	{
		if (strategy == TerranMarineRush)
		{
			return OpeningBuildOrders::TerranMarineRush();
		}
	}
	else if (race == BWAPI::Races::Zerg)
	{
		if (strategy == ZergZerglingRush)
		{
			return OpeningBuildOrders::TerranMarineRush();
		}
	}

	// something wrong, return the protoss one
	BWAPI::Broodwar->printf("Strategy not found, returning empty initial build order");
	return std::vector<MetaType>();
}
const std::vector<MetaType> StrategyManager::getOpeningBookBuildOrder() const
{
	return getOpeningBookBuildOrder(currentStrategy, selfRace);
}

// when do we want to defend with our workers?
// this function can only be called if we have no fighters to defend with
const int StrategyManager::defendWithWorkers()
{
	if (!Options::Micro::WORKER_DEFENSE)
	{
		return false;
	}

	// our home nexus position
	BWAPI::Position homePosition = BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition();;

	// enemy units near our workers
	int enemyUnitsNearWorkers = 0;

	// defense radius of nexus
	int defenseRadius = 300;

	// fill the set with the types of units we're concerned about
	for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		// if it's a zergling or a worker we want to defend
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
		{
			if (unit->getDistance(homePosition) < defenseRadius)
			{
				enemyUnitsNearWorkers++;
			}
		}
	}

	// if there are enemy units near our workers, we want to defend
	return enemyUnitsNearWorkers;
}

// called by combat commander to determine whether or not to send an attack force
// freeUnits are the units available to do this attack
const bool StrategyManager::doAttack(const std::set<BWAPI::UnitInterface*> & freeUnits)
{
	int ourForceSize = (int)freeUnits.size();

	int numUnitsNeededForAttack = 1;

	bool doAttack  = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar) >= 1
					|| ourForceSize >= numUnitsNeededForAttack;

	if (doAttack)
	{
		firstAttackSent = true;
	}

	return doAttack || firstAttackSent;
}

const bool StrategyManager::expandProtossZealotRush() const
{
	return expandProtossZealotRush(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

bool StrategyManager::expandProtossZealotRush(const HLUnitData &unitData, const WorkerData &selfWorkerData, int frame, 
	int frameAdjust, int zealotAdjust)
{
	// if there is no place to expand to, we can't expand
	if (MapTools::Instance().getNextExpansion(unitData.player()) == BWAPI::TilePositions::None)
	{
		return false;
	}

	int numNexus = unitData.getNumUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numZealots = unitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Zealot);


	// if there are more than 10 idle workers, expand
	if (selfWorkerData.getNumIdleWorkers() > 10)
	{
		return true;
	}

	// 2nd Nexus Conditions:
	//		We have 12 or more zealots
	//		It is past frame 7000
	if ((numNexus < 2) && (numZealots >(12 + zealotAdjust) || frame  > (9000 + frameAdjust)))
	{
		return true;
	}

	// 3nd Nexus Conditions:
	//		We have 24 or more zealots
	//		It is past frame 12000
	if ((numNexus < 3) && (numZealots >(24 + zealotAdjust) || frame  > (15000 + frameAdjust)))
	{
		return true;
	}

	if ((numNexus < 4) && (numZealots >(24 + zealotAdjust) || frame  > (21000 + frameAdjust)))
	{
		return true;
	}

	if ((numNexus < 5) && (numZealots >(24 + zealotAdjust) || frame  > (26000 + frameAdjust)))
	{
		return true;
	}

	if ((numNexus < 6) && (numZealots >(24 + zealotAdjust) || frame  > (30000 + frameAdjust)))
	{
		return true;
	}

	return false;
}

const MetaPairVector StrategyManager::getBuildOrderGoal()
{
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (getCurrentStrategy() == ProtossZealotRush)
		{
			return getProtossZealotRushBuildOrderGoal();
		}
		else if (getCurrentStrategy() == ProtossDarkTemplar)
		{
			return getProtossDarkTemplarBuildOrderGoal();
		}
		else if (getCurrentStrategy() == ProtossDragoons)
		{
			return getProtossDragoonsBuildOrderGoal();
		}
		else if (getCurrentStrategy() == ProtossHighLevelSearch)
		{
			return getProtossHighLevelSearchBuildOrderGoal();
		}
		// if something goes wrong, use zealot goal
		return getProtossZealotRushBuildOrderGoal();
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		return getTerranBuildOrderGoal();
	}
	else
	{
		return getZergBuildOrderGoal();
	}
}

MetaPairVector StrategyManager::getBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame,
	int strategy,
	BWAPI::Race race,
	const std::unordered_map<short, short> &choices)
{
	if (race == BWAPI::Races::Protoss)
	{
		switch (strategy){
		case ProtossZealotRush:
			return getProtossZealotRushBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		case ProtossDarkTemplar:
			return getProtossDarkTemplarBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		case ProtossDragoons:
			return getProtossDragoonsBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		case ProtossHighLevelSearch:
			return getProtossHighLevelSearchBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame, choices, false);
			break;
		default:
			UAB_ASSERT(false,"Non existing Protoss strategy %d, using default", strategy);
			//return getProtossZealotRushBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame, choices);
			break;
		}
	}
	else if (race == BWAPI::Races::Terran){
		switch (strategy){
		case TerranMarineRush:
			return getTerranBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		default:
			BWAPI::Broodwar->printf("Non existing Terran strategy %d, using default", strategy);
			return getTerranBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		}
	}
	else if (race == BWAPI::Races::Zerg){
		switch (strategy){
		case ZergZerglingRush:
			return getZergBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		default:
			BWAPI::Broodwar->printf("Non existing Zerg strategy %d, using default", strategy);
			return getZergBuildOrderGoal(selfUnitData, enemyUnitData, selfWorkerData, frame);
			break;
		}
	}
	else{
		UAB_ASSERT(false, "Unknown race when selecting strategy");
		return MetaPairVector();//to silence warning
	}
}

const MetaPairVector StrategyManager::getProtossDragoonsBuildOrderGoal() const
{
	return getProtossDragoonsBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

MetaPairVector StrategyManager::getProtossDragoonsBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame)
{
		// the goal to return
	MetaPairVector goal;

	int numDragoons =			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted =		selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll =			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber =				selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);

	int dragoonsWanted = numDragoons > 0 ? numDragoons + 6 : 2;

	if (enemyUnitData.hasCloakedUnits())
	{

		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));
	
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Observatory) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}
	else
	{
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}

		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Observatory) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}

	if (expandProtossZealotRush(selfUnitData,selfWorkerData,frame))
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon,	dragoonsWanted));
	//goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Gateway,	gatewayWanted));
	//goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Probe,	std::min(90, probesWanted)));

	return goal;
}

const MetaPairVector StrategyManager::getProtossDarkTemplarBuildOrderGoal() const
{
	return getProtossDarkTemplarBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

MetaPairVector StrategyManager::getProtossDarkTemplarBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame)
{
	// the goal to return
	MetaPairVector goal;

	int numDarkTemplar =		selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dark_Templar);
	int numDragoons =			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted =		selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll =			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber =				selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);

	int darkTemplarWanted = 4;
	int dragoonsWanted = numDragoons + 6;



	if (enemyUnitData.hasCloakedUnits())
	{
		
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));
		
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Observatory) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}

	if (numNexusAll >= 2 || frame > 9000)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));
	}

	if (numDragoons > 0)
	{
		goal.push_back(MetaPair(BWAPI::UpgradeTypes::Singularity_Charge, 1));
	}

	if (numNexusCompleted >= 3)
	{
		dragoonsWanted = numDragoons + 6;
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
	}

	if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame))
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon,	dragoonsWanted));
	if (numDarkTemplar < darkTemplarWanted)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dark_Templar, darkTemplarWanted));
	}
	
	return goal;
}

const MetaPairVector StrategyManager::getProtossZealotRushBuildOrderGoal() const
{
	return getProtossZealotRushBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

MetaPairVector StrategyManager::getProtossZealotRushBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame)
{
	// the goal to return
	MetaPairVector goal;

	int numZealots = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Zealot);
	int numDragoons = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted = selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber = selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);

	int zealotsWanted = numZealots + 8;
	int dragoonsWanted = numDragoons;

	if (enemyUnitData.hasCloakedUnits())
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));

		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Observatory) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}

	if (numNexusAll >= 2 || frame > 9000)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Assimilator, 1));
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 1));
	}

	if (numCyber > 0)
	{
		dragoonsWanted = numDragoons + 2;
		goal.push_back(MetaPair(BWAPI::UpgradeTypes::Singularity_Charge, 1));
	}

	if (numNexusCompleted >= 3)
	{
		dragoonsWanted = numDragoons + 6;
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
	}

	if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame))
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, dragoonsWanted));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, zealotsWanted));

	return goal;
}

const MetaPairVector StrategyManager::getProtossHighLevelSearchBuildOrderGoal() const
{
	return getProtossHighLevelSearchBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount(),
		strategyChoices.at(BWAPI::Races::Protoss.getID()).at(ProtossHighLevelSearch),
		true);
}

MetaPairVector StrategyManager::getProtossHighLevelSearchBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame,
	const std::unordered_map<short, short> &choices,
	bool execution)
{
	// the goal to return
	MetaPairVector goal;

	int numZealots = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Zealot);
	int numDragoons = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted = selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber = selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);
	int numDarkTemplar = selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Dark_Templar);

	int zealotsWanted = numZealots;
	int dragoonsWanted = numDragoons;
	int darkTemplarWanted = numDarkTemplar;

	//dragoons or zealots
	int choice = defaultStrategyChoices.at(BWAPI::Races::Protoss.getID()).at(ProtossHighLevelSearch).at(0);
	if (choices.find(0) == choices.end() )
	{
		if (!execution)
		{
			throw ChoicePoint(0, 4);
		}
	}
	else
	{
		choice = choices.at(0);
	}
	switch (choice){
	case 0:
		zealotsWanted += 8;
		break;
	case 1:
		dragoonsWanted += 6;
		break;
	case 2:
		zealotsWanted += 4;
		dragoonsWanted += 3; 
		break;
	case 3:
		darkTemplarWanted = 4;
		if (numDarkTemplar >= 3)
		{
			dragoonsWanted += 6;
			//darkTemplarWanted = numDarkTemplar + 1;
		} 
		break;
	default:
		UAB_ASSERT(false, "Wrong choice point option");
	}

	if (numDragoons < dragoonsWanted) goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, dragoonsWanted));
	if (numZealots < zealotsWanted) goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, zealotsWanted));
	if (numDarkTemplar < darkTemplarWanted) goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dark_Templar, darkTemplarWanted));

	if (enemyUnitData.hasCloakedUnits())
	{
		if (selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon) < 2)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Photon_Cannon, 2));
		}
		if (selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) < 1)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));
		}
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0 &&
			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Observatory) < 1)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}
		if (selfUnitData.getNumCompletedUnits(BWAPI::UnitTypes::Protoss_Observatory) > 0 &&
			selfUnitData.getNumUnits(BWAPI::UnitTypes::Protoss_Observer) < 1)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}

	if (numNexusAll >= 2 || frame > 9000)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Assimilator, 1));
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 1));
	}

	if ((numCyber > 0) && ((numDragoons + dragoonsWanted)>0))
	{
		//dragoonsWanted = numDragoons + 2;
		goal.push_back(MetaPair(BWAPI::UpgradeTypes::Singularity_Charge, 1));
	}

	if (numNexusCompleted >= 3)
	{
		//dragoonsWanted = numDragoons + 6;
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
	}


	if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame, 0, 0))
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}
	//if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame, -2, -1000))
	//{
	//	//choice 1: expand or not
	//	int choice = defaultStrategyChoices.at(BWAPI::Races::Protoss.getID()).at(ProtossZealotRush).at(1);
	//	if (choices.find(1) == choices.end())
	//	{
	//		if (!execution)
	//		{
	//			throw ChoicePoint(1, 3);
	//		}
	//	}
	//	else
	//	{
	//		choice = choices.at(1);
	//	}
	//	switch (choice){
	//	case 0://aggressive
	//		//if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame, -2, -1000))
	//		//{
	//			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	//		//}
	//		break;
	//	case 1://normal
	//		if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame, 0, 0))
	//		{
	//			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	//		}
	//		break;
	//	case 2://passive
	//		if (expandProtossZealotRush(selfUnitData, selfWorkerData, frame, 2, 1000))
	//		{
	//			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	//		}
	//		break;
	//	default:
	//		UAB_ASSERT(false, "Wrong choice point option");
	//	}
	//}

	return goal;
}


const MetaPairVector StrategyManager::getTerranBuildOrderGoal() const
{
	return getTerranBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

MetaPairVector StrategyManager::getTerranBuildOrderGoal(
	const HLUnitData &selfUnitData,
	const HLUnitData &enemyUnitData,
	const WorkerData &selfWorkerData,
	int frame)
{
	// the goal to return
	std::vector<MetaPair> goal;

	int numMarines =			selfUnitData.getNumUnits(BWAPI::UnitTypes::Terran_Marine);
	int numMedics =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Terran_Medic);
	int numWraith =				selfUnitData.getNumUnits(BWAPI::UnitTypes::Terran_Wraith);

	int marinesWanted = numMarines + 12;
	int medicsWanted = numMedics + 2;
	int wraithsWanted = numWraith + 4;

	goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Marine,	marinesWanted));

	return (const std::vector<MetaPair>)goal;
}

const MetaPairVector StrategyManager::getZergBuildOrderGoal() const
{
	return getZergBuildOrderGoal(
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->self()), BWAPI::Broodwar->self()),
		HLUnitData(InformationManager::Instance().getUnitData(BWAPI::Broodwar->enemy()), BWAPI::Broodwar->enemy()),
		WorkerManager::Instance().getData(),
		BWAPI::Broodwar->getFrameCount());
}

MetaPairVector StrategyManager::getZergBuildOrderGoal(const HLUnitData &selfUnitData, const HLUnitData &enemyUnitData, const WorkerData &selfWorkerData, int frame)
{
	// the goal to return
	std::vector<MetaPair> goal;
	
	int numMutas  = selfUnitData.getNumUnits(BWAPI::UnitTypes::Zerg_Mutalisk);
	int numHydras = selfUnitData.getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk);

	int mutasWanted = numMutas + 6;
	int hydrasWanted = numHydras + 6;

	goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Zergling, 4));
	//goal.push_back(std::pair<MetaType, int>(BWAPI::TechTypes::Stim_Packs,	1));

	//goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Medic,		medicsWanted));

	return (const std::vector<MetaPair>)goal;
}

 const int StrategyManager::getCurrentStrategy() const
 {
	 return currentStrategy;
 }

 void StrategyManager::setCurrentStrategy(int newStrategy, const std::unordered_map<short, short> &choices)
 {
	 currentStrategy = newStrategy;
	 strategyChoices[BWAPI::Broodwar->self()->getRace().getID()][currentStrategy] = choices;
 }

 int StrategyManager::getNumStrategies(BWAPI::Race race)
 {
	 if (race == BWAPI::Races::Protoss)
	 {
		 return NumProtossStrategies;
	 }
	 else if (race == BWAPI::Races::Zerg){
		 return NumZergStrategies;
	 }
	 else if (race == BWAPI::Races::Terran){
		 return NumTerranStrategies;
	 }
	 else{
		 UAB_ASSERT(false, "Unknown race");
	 }
 }