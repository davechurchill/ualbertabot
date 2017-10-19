#include "Common.h"
#include "StrategyManager.h"
#include "UnitUtil.h"
#include "ParseUtils.h"
#include "FileLogger.h"

using namespace UAlbertaBot;

// constructor
StrategyManager::StrategyManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<UnitInfoManager> unitInfo,
	shared_ptr<BaseLocationManager> bases,
	std::shared_ptr<AKBot::Logger> logger,
	BotStrategyConfiguration& strategyConfiguration)
	: _strategyName("")
	, _opponentView(opponentView)
	, _unitInfo(unitInfo)
	, _bases(bases)
	, _emptyBuildOrder(opponentView->self()->getRace())
	, _logger(logger)
	, _strategyConfiguration(strategyConfiguration)
{
}

void StrategyManager::update()
{

}

const int StrategyManager::getScore(BWAPI::Player player) const
{
	return player->getBuildingScore() + player->getKillScore() + player->getRazingScore() + player->getUnitScore();
}

const BuildOrder & StrategyManager::getOpeningBookBuildOrder() const
{
    auto buildOrderIt = _strategies.find(_strategyName);

    // look for the build order in the build order map
	if (buildOrderIt != std::end(_strategies))
    {
        return (*buildOrderIt).second._buildOrder;
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Strategy not found: %s, returning empty initial build order", _strategyName.c_str());
        return _emptyBuildOrder;
    }
}

const bool StrategyManager::shouldExpandNow(int currentFrame) const
{
	// if there is no place to expand to, we can't expand
	if (_bases->getNextExpansion(_opponentView->self()) == BWAPI::TilePositions::None)
	{
        _logger->log("No valid expansion location");
		return false;
	}

	size_t numDepots    = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Command_Center)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Nexus)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hatchery)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Lair)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hive);
	int frame           = currentFrame;
    int minute          = frame / (24*60);

    // if we have a ridiculous stockpile of minerals, expand
    if (_opponentView->self()->minerals() > 3000)
    {
        return true;
    }

    // we will make expansion N after array[N] minutes have passed
    std::vector<int> expansionTimes = {5, 10, 20, 30, 40 , 50};

    for (size_t i(0); i < expansionTimes.size(); ++i)
    {
        if (numDepots < (i+2) && minute > expansionTimes[i])
        {
            return true;
        }
    }

	return false;
}

void StrategyManager::addStrategy(const std::string & name, Strategy & strategy)
{
    _strategies[name] = strategy;
}

const MetaPairVector StrategyManager::getBuildOrderGoal(int currentFrame) const
{
    auto myRace = _opponentView->self()->getRace();

    if (myRace == BWAPI::Races::Protoss)
    {
        return getProtossBuildOrderGoal(currentFrame);
    }
    else if (myRace == BWAPI::Races::Terran)
	{
		return getTerranBuildOrderGoal(currentFrame);
	}
    else if (myRace == BWAPI::Races::Zerg)
	{
		return getZergBuildOrderGoal(currentFrame);
	}

    return MetaPairVector();
}

const MetaPairVector StrategyManager::getProtossBuildOrderGoal(int currentFrame) const
{
	// the goal to return
	MetaPairVector goal;

	int numZealots          = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Zealot);
    int numPylons           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Pylon);
	int numDragoons         = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Dragoon);
	int numProbes           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Probe);
	int numNexusCompleted   = _opponentView->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Nexus);
	int numNexusAll         = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Nexus);
	int numCyber            = _opponentView->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	int numCannon           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Photon_Cannon);
    int numScout            = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Corsair);
    int numReaver           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Reaver);
    int numDarkTeplar       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar);

    if (_strategyName == "Protoss_ZealotRush")
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));

        // once we have a 2nd nexus start making dragoons
        if (numNexusAll >= 2)
        {
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 4));
        }
    }
    else if (_strategyName == "Protoss_DragoonRush")
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 6));
    }
    else if (_strategyName == "Protoss_Drop")
    {
        if (numZealots == 0)
        {
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 4));
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Shuttle, 1));
        }
        else
        {
            goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));
        }
    }
    else if (_strategyName == "Protoss_DTRush")
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
        UAB_ASSERT_WARNING(false, "Unknown Protoss Strategy Name: %s", _strategyName.c_str());
    }

    // if we have 3 nexus, make an observer
    if (numNexusCompleted >= 3)
    {
        goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
    }
    
    // add observer to the goal if the enemy has cloaked units
	if (_unitInfo->enemyHasCloakedUnits())
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Observer, 1));
	}

    // if we want to expand, insert a nexus into the build order
	if (shouldExpandNow(currentFrame))
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Nexus, numNexusAll + 1));
	}

	return goal;
}

const MetaPairVector StrategyManager::getTerranBuildOrderGoal(int currentFrame) const
{
	// the goal to return
	std::vector<MetaPair> goal;

    int numWorkers      = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_SCV);
    int numCC           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Command_Center);            
    int numMarines      = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Marine);
	int numMedics       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Medic);
	int numWraith       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Wraith);
    int numVultures     = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Vulture);
    int numGoliath      = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Goliath);
    int numTanks        = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
    int numBay          = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Terran_Engineering_Bay);

    if (_strategyName == "Terran_MarineRush")
    {
	    goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));

        if (numMarines > 5)
        {
            goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Engineering_Bay, 1));
        }
    }
    else if (_strategyName == "Terran_4RaxMarines")
    {
	    goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));
    }
    else if (_strategyName == "Terran_VultureRush")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Vulture, numVultures + 8));

        if (numVultures > 8)
        {
            goal.push_back(std::pair<MetaType, int>(BWAPI::TechTypes::Tank_Siege_Mode, 1));
            goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 4));
        }
    }
    else if (_strategyName == "Terran_TankPush")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 6));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Goliath, numGoliath + 6));
        goal.push_back(std::pair<MetaType, int>(BWAPI::TechTypes::Tank_Siege_Mode, 1));
    }
    else
    {
        _logger->log("Warning: No build order goal for Terran Strategy: %s", _strategyName.c_str());
    }

    if (shouldExpandNow(currentFrame))
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_Command_Center, numCC + 1));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Terran_SCV, numWorkers + 10));
    }

    if (_unitInfo->enemyHasCloakedUnits())
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Science_Vessel, 1));
	}

	return goal;
}

const MetaPairVector StrategyManager::getZergBuildOrderGoal(int currentFrame) const
{
	// the goal to return
	std::vector<MetaPair> goal;
	
    int numWorkers      = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Drone);
    int numCC           = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hatchery)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Lair)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hive);
	int numMutas        = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Mutalisk);
    int numDrones       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Drone);
    int zerglings       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Zergling);
	int numHydras       = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk);
    int numScourge      = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Scourge);
    int numGuardians    = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Guardian);

	int mutasWanted = numMutas + 6;
	int hydrasWanted = numHydras + 6;

    if (_strategyName == "Zerg_ZerglingRush")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Zergling, zerglings + 6));
    }
    else if (_strategyName == "Zerg_2HatchHydra")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 8));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UpgradeTypes::Grooved_Spines, 1));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
    }
    else if (_strategyName == "Zerg_3HatchMuta")
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 12));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
    }
    else if (_strategyName == "Zerg_3HatchScourge")
    {
        if (numScourge > 40)
        {
            goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 12));
        }
        else
        {
            goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Scourge, numScourge + 12));
        }

        
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
    }

    if (shouldExpandNow(currentFrame))
    {
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Hatchery, numCC + 1));
        goal.push_back(std::pair<MetaType, int>(BWAPI::UnitTypes::Zerg_Drone, numWorkers + 10));
    }

	return goal;
}

void StrategyManager::readResults()
{
    if (!_strategyConfiguration.UsingStrategyIO)
    {
        return;
    }

	// If we don't have any enemy
	auto enemy = _opponentView->defaultEnemy();
	if (enemy == nullptr)
	{
		return;
	}

    std::string enemyName = enemy->getName();
    std::replace(enemyName.begin(), enemyName.end(), ' ', '_');

    std::string enemyResultsFile = _strategyConfiguration.ReadDir + enemyName + ".txt";
    
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

            //_logger.log("Results Found: %s %d %d", strategyName.c_str(), wins, losses);

			auto strategyPtr = _strategies.find(strategyName);
            if (strategyPtr == _strategies.end())
            {
                //_logger.log("Warning: Results file has unknown Strategy: %s", strategyName.c_str());
            }
            else
            {
				auto& strategy = strategyPtr->second;
                strategy._wins = wins;
                strategy._losses = losses;
            }
        }

        fclose ( file );
    }
    else
    {
        //_logger.log("No results file found: %s", enemyResultsFile.c_str());
    }
}

void StrategyManager::writeResults()
{
    if (!_strategyConfiguration.UsingStrategyIO)
    {
        return;
    }

	auto enemy = _opponentView->defaultEnemy();
	if (enemy == nullptr)
	{
		return;
	}

	std::string enemyName = _opponentView->defaultEnemy()->getName();
    std::replace(enemyName.begin(), enemyName.end(), ' ', '_');

    std::string enemyResultsFile = _strategyConfiguration.WriteDir + enemyName + ".txt";

    std::stringstream ss;

    for (auto & kv : _strategies)
    {
        const Strategy & strategy = kv.second;

        ss << strategy._name << " " << strategy._wins << " " << strategy._losses << "\n";
    }

    Logger::LogOverwriteToFile(enemyResultsFile, ss.str());
}

void StrategyManager::onEnd(const bool isWinner)
{
    if (!_strategyConfiguration.UsingStrategyIO)
    {
        return;
    }

    if (isWinner)
    {
        _strategies[_strategyName]._wins++;
    }
    else
    {
        _strategies[_strategyName]._losses++;
    }

    writeResults();
}

void StrategyManager::setLearnedStrategy()
{
    // we are currently not using this functionality for the competition so turn it off 
    return;

    if (!_strategyConfiguration.UsingStrategyIO)
    {
        return;
    }

    const std::string & strategyName = _strategyName;
    Strategy & currentStrategy = _strategies[strategyName];

    int totalGamesPlayed = 0;
    int strategyGamesPlayed = currentStrategy._wins + currentStrategy._losses;
    double winRate = strategyGamesPlayed > 0 ? currentStrategy._wins / static_cast<double>(strategyGamesPlayed) : 0;

    // if we are using an enemy specific strategy
    if (_strategyConfiguration.FoundEnemySpecificStrategy)
    {        
        return;
    }

    // if our win rate with the current strategy is super high don't explore at all
    // also we're pretty confident in our base strategies so don't change if insufficient games have been played
    if (strategyGamesPlayed < 5 || (strategyGamesPlayed > 0 && winRate > 0.49))
    {
        _logger->log("Still using default strategy");
        return;
    }

    // get the total number of games played so far with this race
    for (auto & kv : _strategies)
    {
        auto& strategy = kv.second;
        if (strategy._race == _opponentView->self()->getRace())
        {
            totalGamesPlayed += strategy._wins + strategy._losses;
        }
    }

    // calculate the UCB value and store the highest
    double C = 0.5;
    std::string bestUCBStrategy;
    double bestUCBStrategyVal = std::numeric_limits<double>::lowest();
    for (auto & kv : _strategies)
    {
        Strategy & strategy = kv.second;
        if (strategy._race != _opponentView->self()->getRace())
        {
            continue;
        }

        int sGamesPlayed = strategy._wins + strategy._losses;
        double sWinRate = sGamesPlayed > 0 ? currentStrategy._wins / static_cast<double>(strategyGamesPlayed) : 0;
        double ucbVal = C * sqrt( log( (double)totalGamesPlayed / sGamesPlayed ) );
        double val = sWinRate + ucbVal;

        if (val > bestUCBStrategyVal)
        {
            bestUCBStrategy = strategy._name;
            bestUCBStrategyVal = val;
        }
    }

    _strategyName = bestUCBStrategy;

	// This is to beckward compatibility to be able change some places later on.
	_strategyConfiguration.StrategyName = bestUCBStrategy;
}

void UAlbertaBot::StrategyManager::setPreferredStrategy(std::string strategy)
{
	_strategyName = strategy;
}
