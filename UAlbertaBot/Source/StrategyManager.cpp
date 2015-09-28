#include "Common.h"
#include "StrategyManager.h"

using namespace UAlbertaBot;

// constructor
StrategyManager::StrategyManager() 
	: firstAttackSent(false)
	, _selfRace(BWAPI::Broodwar->self()->getRace())
	, _enemyRace(BWAPI::Broodwar->enemy()->getRace())
    , _emptyBuildOrder(BWAPI::Broodwar->self()->getRace())
{
	if (Config::Modules::UsingStrategyIO)
	{
		readResults();
	}
}

// get an instance of this
StrategyManager & StrategyManager::Instance() 
{
	static StrategyManager instance;
	return instance;
}


const int StrategyManager::getScore(BWAPI::PlayerInterface * player) const
{
	return player->getBuildingScore() + player->getKillScore() + player->getRazingScore() + player->getUnitScore();
}

const BuildOrder & StrategyManager::getOpeningBookBuildOrder() const
{
    auto buildOrderIt = _openingBuildOrders.find(Config::Strategy::StrategyName);

    // look for the build order in the build order map
	if (buildOrderIt != std::end(_openingBuildOrders))
    {
        return (*buildOrderIt).second;
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Strategy not found: %s, returning empty initial build order", Config::Strategy::StrategyName.c_str());
        return _emptyBuildOrder;
    }
}



// called by combat commander to determine whether or not to send an attack force
// freeUnits are the units available to do this attack
const bool StrategyManager::doAttack(const std::set<BWAPI::Unit> & freeUnits)
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

const bool StrategyManager::shouldExpandNow() const
{
	// if there is no place to expand to, we can't expand
	if (MapTools::Instance().getNextExpansion() == BWAPI::TilePositions::None)
	{
        BWAPI::Broodwar->printf("No valid expansion location");
		return false;
	}

	size_t numDepots    = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Command_Center)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Nexus)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Hatchery)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Lair)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Hive);
	int frame           = BWAPI::Broodwar->getFrameCount();
    int minute          = frame / (24*60);

	// if we have a ton of idle workers then we need a new expansion
	if (WorkerManager::Instance().getNumIdleWorkers() > 10)
	{
		return true;
	}

    // if we have a ridiculous stockpile of minerals, expand
    if (BWAPI::Broodwar->self()->minerals() > 3000)
    {
        return true;
    }

    // we will make expansion N after array[N] minutes have passed
    std::vector<int> expansionTimes = {5, 10, 20, 30, 40 , 50};

    for (size_t i(0); i < expansionTimes.size(); ++i)
    {
        if (numDepots < (i+2) && minute > expansionTimes[i])
        {
            BWAPI::Broodwar->printf("We should expand");
            return true;
        }
    }

	return false;
}

void StrategyManager::addOpeningBuildOrder(const std::string & name, BuildOrder & buildOrder)
{
    _openingBuildOrders[name] = buildOrder;
}

const MetaPairVector StrategyManager::getBuildOrderGoal()
{
    BWAPI::Race myRace = BWAPI::Broodwar->self()->getRace();

    if (myRace == BWAPI::Races::Protoss)
    {
        return getProtossBuildOrderGoal();
    }
    else if (myRace == BWAPI::Races::Terran)
	{
		return getTerranBuildOrderGoal();
	}
    else if (myRace == BWAPI::Races::Zerg)
	{
		return getZergBuildOrderGoal();
	}

    return MetaPairVector();
}

const MetaPairVector StrategyManager::getProtossBuildOrderGoal() const
{
	// the goal to return
	MetaPairVector goal;

	int numZealots          = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Zealot);
	int numDragoons         = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes           = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted   = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll         = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber            = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon           = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Photon_Cannon);
    int numScout            = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Corsair);
    int numReaver           = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Reaver);
    int numDarkTeplar       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar);

    if (Config::Strategy::StrategyName == "Protoss_ZealotRush")
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));

        // once we have a 2nd nexus start making dragoons
        if (numNexusAll >= 2)
        {
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 4));
        }
    }
    else if (Config::Strategy::StrategyName == "Protoss_DragoontRush")
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 6));
    }
    else if (Config::Strategy::StrategyName == "Protoss_DTRush")
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dark_Templar, numDarkTeplar + 2));

        // if we have a 2nd nexus then get some goons out
        if (numNexusAll >= 2)
        {
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 4));
        }
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Unknown Protoss Strategy Name: %s", Config::Strategy::StrategyName.c_str());
    }

    // if we have 3 nexus, make an observer
    if (numNexusCompleted >= 3)
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
    }
    
    // add observer to the goal if the enemy has cloaked units
	if (InformationManager::Instance().enemyHasCloakedUnits())
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1));
		
		if (BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observatory, 1));
		}
		if (BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Observatory) > 0)
		{
			goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
		}
	}

    // if we want to expand, insert a nexus into the build order
	if (shouldExpandNow())
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}

	return goal;
}


const MetaPairVector StrategyManager::getTerranBuildOrderGoal() const
{
	// the goal to return
	std::vector<MetaPair> goal;

    int numWorkers      = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_SCV);
    int numCC           = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Command_Center);            
    int numMarines      = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Marine);
	int numMedics       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Medic);
	int numWraith       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Wraith);
    int numVultures     = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Vulture);

    if (Config::Strategy::StrategyName == "Terran_MarineRush")
    {
	    goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Marine, numMarines + 13));
    }
    else if (Config::Strategy::StrategyName == "Terran_VultureRush")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Vulture, numVultures + 8));
        goal.push_back(std::pair<MetaType, int>(BWAPI::TechTypes::Spider_Mines, 1));
    }

    if (shouldExpandNow())
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Command_Center, numCC + 1));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_SCV, numWorkers + 10));
    }

	return goal;
}

const MetaPairVector StrategyManager::getZergBuildOrderGoal() const
{
	// the goal to return
	std::vector<MetaPair> goal;
	
    int numWorkers      = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Drone);
    int numCC           = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Hatchery)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Lair)
                        + BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Hive);
	int numMutas        = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Mutalisk);
    int numDrones       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Drone);
    int zerglings       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Zergling);
	int numHydras       = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk);

	int mutasWanted = numMutas + 6;
	int hydrasWanted = numHydras + 6;

    if (Config::Strategy::StrategyName == "Zerg_ZerglingRush")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Zergling, zerglings + 6));
    }
    else if (Config::Strategy::StrategyName == "Zerg_2HatchHydra")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 8));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
    }
    else if (Config::Strategy::StrategyName == "Zerg_3HatchMuta")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Mutalisk, numMutas + 8));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
    }

	return (const std::vector<MetaPair>)goal;
}

void StrategyManager::readResults()
{
    if (!Config::Modules::UsingStrategyIO)
    {
        return;
    }

    std::string enemyName = BWAPI::Broodwar->enemy()->getName();
    std::replace(enemyName.begin(), enemyName.end(), ' ', '_');

    std::string enemyResultsFile = Config::Strategy::ReadDir + enemyName + ".txt";
    
    std::string strategyName;
    int wins = 0;
    int losses = 0;

    FILE *file = fopen ( enemyResultsFile.c_str(), "r" );
    if ( file != nullptr )
    {
        char line [ 4096 ]; /* or other suitable maximum line size */
        while ( fgets ( line, sizeof line, file ) != nullptr ) /* read a line */
        {
            std::stringstream ss(line);

            ss >> strategyName;
            ss >> wins;
            ss >> losses;

            BWAPI::Broodwar->printf("Results Found: %s %d %d", strategyName.c_str(), wins, losses);

            _results[strategyName] = std::pair<int, int>(wins, losses);
        }

        fclose ( file );
    }
    else
    {
        BWAPI::Broodwar->printf("No results file found: %s", enemyResultsFile.c_str());
    }

    // if we haven't seen the current strategy yet in the results file, add it
    if (_results.find(Config::Strategy::StrategyName) == _results.end())
    {
        _results[Config::Strategy::StrategyName] = std::pair<int, int>(0, 0);
    }
}

void StrategyManager::writeResults()
{
    if (!Config::Modules::UsingStrategyIO)
    {
        return;
    }

    std::string enemyName = BWAPI::Broodwar->enemy()->getName();
    std::replace(enemyName.begin(), enemyName.end(), ' ', '_');

    std::string enemyResultsFile = Config::Strategy::WriteDir + enemyName + ".txt";

    std::stringstream ss;

    for (auto & kv : _results)
    {
        ss << kv.first << " " << kv.second.first << " " << kv.second.second << "\n";
    }

    Logger::LogOverwriteToFile(enemyResultsFile, ss.str());
}

void StrategyManager::onEnd(const bool isWinner)
{
    if (!Config::Modules::UsingStrategyIO)
    {
        return;
    }

    if (isWinner)
    {
        _results[Config::Strategy::StrategyName].first++;
    }
    else
    {
        _results[Config::Strategy::StrategyName].second++;
    }

    writeResults();
}
