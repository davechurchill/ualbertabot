#include "Common.h"
#include "BOSSManager.h"
#include "UnitUtil.h"
#include "FileLogger.h"

using namespace UAlbertaBot;

// constructor
BOSSManager::BOSSManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	const BotDebugConfiguration& debugConfiguration)
	: _opponentView(opponentView)
	, _previousSearchStartFrame(0)
    , _previousSearchFinishFrame(0)
    , _searchInProgress(false)
    , _previousStatus("No Searches")
	, _debugConfiguration(debugConfiguration)
{
	
}

void BOSSManager::reset()
{
    _previousSearchResults = BOSS::DFBB_BuildOrderSearchResults();
    _searchInProgress = false;
    _previousBuildOrder.clear();
}

// start a new search for a new goal
void BOSSManager::startNewSearch(const std::vector<MetaPair> & goalUnits, shared_ptr<BuildingManager> buildingManager, int currentFrame)
{
	auto self = _opponentView->self();
    size_t numWorkers   = UnitUtil::GetAllUnitCount(self, self->getRace().getWorker());
    size_t numDepots    = UnitUtil::GetAllUnitCount(self, UnitUtil::getResourceDepot(self->getRace()))
                        + UnitUtil::GetAllUnitCount(self, BWAPI::UnitTypes::Zerg_Lair)
                        + UnitUtil::GetAllUnitCount(self, BWAPI::UnitTypes::Zerg_Hive);

    if (numWorkers == 0)
    {
        _previousStatus = "\x08No Workers :(";
        return;
    }

    if (numDepots == 0)
    {
        _previousStatus = "\x08No Depots :(";
        return;
    }

    // convert from UAlbertaBot's meta goal type to BOSS ActionType goal
    try
    {
        BOSS::BuildOrderSearchGoal goal = GetGoal(self, goalUnits);

        BOSS::GameState initialState(currentFrame, _opponentView->self(), buildingManager->buildingsQueued());

        _smartSearch = SearchPtr(new BOSS::DFBB_BuildOrderSmartSearch(initialState.getRace()));
        _smartSearch->setGoal(GetGoal(self, goalUnits));
        _smartSearch->setState(initialState);

        _searchInProgress = true;
        _previousSearchStartFrame = currentFrame;
        _totalPreviousSearchTime = 0;
        _previousGoalUnits = goalUnits;
    }
    catch (const BOSS::BOSSException)
    {
        BWAPI::BroodwarPtr->printf("Exception in BOSS::GameState constructor, will try again next frame");
    }
}

// tell the search to keep going for however long we have this frame
void BOSSManager::update(double timeLimit, int currentFrame)
{
    // if there's a search in progress, resume it
    if (isSearchInProgress())
    {
        _previousStatus.clear();

        // give the search at least 5ms to search this frame
        double realTimeLimit = timeLimit < 0 ? 5 : timeLimit;
        _smartSearch->setTimeLimit((int)realTimeLimit);
		_hasExceptionDuringSearch = false;

		try
        {
            // call the search to continue searching
            // this will resume a search in progress or start a new search if not yet started
			_smartSearch->search();
		}
        // catch any errors that might happen in the search
		catch (const BOSS::BOSSException & exception)
        {
            UAB_ASSERT_WARNING(false, "BOSS SmartSearch Exception: %s", exception.what());
            _previousStatus = "BOSSExeption";
			_hasExceptionDuringSearch = true;
		}

		auto& searchResults = _smartSearch->getResults();
        _totalPreviousSearchTime += searchResults.timeElapsed;

        // after the search finishes for this frame, check to see if we have a solution or if we hit the overall time limit
        bool searchTimeOut = currentFrame > (_previousSearchStartFrame + _bossFrameLimit);
        bool previousSearchComplete = searchTimeOut || searchResults.solved || _hasExceptionDuringSearch;
        if (previousSearchComplete)
        {
            bool solved = searchResults.solved && searchResults.solutionFound;

            // if we've found a solution, let us know
            if (searchResults.solved)
            {
                if (searchResults.solutionFound)
                {
                    _previousStatus = std::string("\x07") + "BOSS Solve Solution\n";
                }
                else
                {
                    _previousStatus = std::string("\x03") + "BOSS Solve NoSolution\n";
                }
            }

            // re-set all the search information to get read for the next search
            _searchInProgress = false;
            _previousSearchFinishFrame = currentFrame;
            _previousSearchResults = _smartSearch->getResults();
            _savedSearchResults = _previousSearchResults;
            _previousBuildOrder = _previousSearchResults.buildOrder;

            if (solved && _previousBuildOrder.size() == 0)
            {
                _previousStatus = std::string("\x07") + "BOSS Trivial Solve\n";
            }

            // if our search resulted in a build order of size 0 then something failed
			_noSolutionFound = false;
            if (!solved && _previousBuildOrder.size() == 0)
            {
                // log the debug information since this shouldn't happen if everything goes to plan
                /*std::stringstream ss;
                ss << _smartSearch->getParameters().toString() << "\n";
                ss << "searchTimeOut: " << (searchTimeOut ? "true" : "false") << "\n";
                ss << "caughtException: " << (caughtException ? "true" : "false") << "\n";
                ss << "getResults().solved: " << (_smartSearch->getResults().solved ? "true" : "false") << "\n";
                ss << "getResults().solutionFound: " << (_smartSearch->getResults().solutionFound ? "true" : "false") << "\n";
                ss << "nodes: " << _savedSearchResults.nodesExpanded << "\n";
                ss << "time: " << _savedSearchResults.timeElapsed << "\n";
                Logger::LogOverwriteToFile("bwapi-data/AI/LastBadBuildOrder.txt", ss.str());*/
                
                // so try another naive build order search as a last resort
                BOSS::NaiveBuildOrderSearch nbos(_smartSearch->getParameters().initialState, _smartSearch->getParameters().goal);

				try
                {
                    if (searchTimeOut)
                    {
                        _previousStatus = std::string("\x02") + "BOSS Timeout\n";
                    }

                    if (_hasExceptionDuringSearch)
                    {
                        _previousStatus = std::string("\x02") + "BOSS Exception\n";
                    }

					_previousBuildOrder = nbos.solve();
                    _previousStatus += "\x03NBOS Solution";

					return;
				}
                // and if that search doesn't work then we're out of luck, no build orders forus
				catch (const BOSS::BOSSException & exception)
                {
                    UAB_ASSERT_WARNING(false, "BOSS Timeout Naive Search Exception: %s", exception.what());
                    _previousStatus += "\x08Naive Exception";
					_noSolutionFound = true;
					_previousBuildOrder = BOSS::BuildOrder();
					return;
				}
            }
        }
    }
}

void BOSSManager::logBadSearch()
{
    std::string s = _smartSearch->getParameters().toString();

    Logger::LogOverwriteToFile("c:/uaberror.txt", s);
}

BOSS::BuildOrderSearchGoal BOSSManager::GetGoal(BWAPI::Player self, const std::vector<MetaPair> & goalUnits)
{
	BOSS::BuildOrderSearchGoal goal(BOSS::Races::GetRaceID(self->getRace()));

	for (size_t i=0; i<goalUnits.size(); ++i)
	{
		goal.setGoal(GetActionType(goalUnits[i].first), (BOSS::UnitCountType)goalUnits[i].second);
	}

	return goal;
}

// gets the StarcraftState corresponding to the beginning of a Melee game
BOSS::GameState BOSSManager::getStartState()
{
    BOSS::GameState state(getRace());
    state.setStartingState();

	return state;
}

const BOSS::RaceID BOSSManager::getRace() const
{
    BWAPI::Race r = _opponentView->self()->getRace();
    if (r == BWAPI::Races::Protoss)
    {
        return BOSS::Races::Protoss;
    }
    else if (r == BWAPI::Races::Terran)
    {
        return BOSS::Races::Terran;
    }
    else if (r == BWAPI::Races::Zerg)
    {
        return BOSS::Races::Zerg;
    }
    else
    {
        BOSS_ASSERT(false, "We should have had a valid race from BWAPI");
        return BOSS::Races::None;
    }
}

bool BOSSManager::isSearchInProgress()
{
    return _searchInProgress;
}

// converts SearchResults.buildOrder vector into vector of MetaType
std::vector<MetaType> BOSSManager::GetMetaVector(const BOSS::BuildOrder & buildOrder)
{
	std::vector<MetaType> metaVector;
    	
	for (size_t i(0); i<buildOrder.size(); ++i)
	{
		metaVector.push_back(GetMetaType(buildOrder[i]));
	}

	return metaVector;
}


BuildOrder BOSSManager::getBuildOrder()
{
    return BuildOrder(_opponentView->self()->getRace(), GetMetaVector(_previousBuildOrder));
}

BOSS::ActionType BOSSManager::GetActionType(const MetaType & t)
{
	// set the appropriate type
	if (t.isUnit())
	{
		return BOSS::ActionType(t.getUnitType());
	}
	else if (t.isUpgrade())
	{
		return BOSS::ActionType(t.getUpgradeType());
	} 
	else if (t.isTech())
	{
		return BOSS::ActionType(t.getTechType());
	}
	else
	{
		UAB_ASSERT(false, "Should have found a valid type here");
	}
	
	return BOSS::ActionType();
}

MetaType BOSSManager::GetMetaType(const BOSS::ActionType & a)
{
	// set the appropriate type
	if (a.isUnit())
	{
		return MetaType(a.getUnitType());
	}
	else if (a.isUpgrade())
	{
		return MetaType(a.getUpgradeType());
	} 
	else if (a.isTech())
	{
		return MetaType(a.getTechType());
	}
	else
	{
		UAB_ASSERT(false, "Should have found a valid type here");
	}
	
	return MetaType();
}
