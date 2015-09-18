#include "Common.h"
#include "BOSSManager.h"

using namespace UAlbertaBot;

// get an instance of this
BOSSManager & BOSSManager::Instance() 
{
	static BOSSManager instance;
	return instance;
}

// constructor
BOSSManager::BOSSManager() 
	: _previousSearchStartFrame(0)
    , _previousSearchFinishFrame(0)
    , _searchInProgress(false)
    , _previousStatus("No Searches")
{
	
}

void BOSSManager::reset()
{
    _previousSearchResults = BOSS::DFBB_BuildOrderSearchResults();
    _searchInProgress = false;
    _previousBuildOrder.clear();
}

// start a new search for a new goal
void BOSSManager::startNewSearch(const std::vector<MetaPair> & goalUnits)
{
    // convert from UAlbertaBot's meta goal type to BOSS ActionType goal
    BOSS::BuildOrderSearchGoal goal = GetGoal(goalUnits);
    BOSS::GameState initialState(BWAPI::Broodwar, BWAPI::Broodwar->self(), BuildingManager::Instance().buildingsQueued());

    Logger::LogAppendToFile(Config::Debug::ErrorLogFilename, initialState.toString());

    _smartSearch = SearchPtr(new BOSS::DFBB_BuildOrderSmartSearch(initialState.getRace()));
    _smartSearch->setGoal(GetGoal(goalUnits));
    _smartSearch->setState(initialState);

    _searchInProgress = true;
    _previousSearchStartFrame = BWAPI::Broodwar->getFrameCount();
    _totalPreviousSearchTime = 0;
    _previousGoalUnits = goalUnits;
}

void BOSSManager::drawSearchInformation(int x, int y) 
{
	if (!Config::Debug::DrawBuildOrderSearchInfo)
    {
        return;
    }

    // draw the background
    int width = 120;
    int height = 100;
    BWAPI::Broodwar->drawBoxScreen(BWAPI::Position(x,y), BWAPI::Position(x+width, y+height), BWAPI::Colors::Black, true);

    x += 5; y+=3;

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y), "BuildOrderSearch:");
    y += 10;
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y), "%s", _previousStatus.c_str());

    for (size_t i(0); i < _previousGoalUnits.size(); ++i)
    {
        if (_previousGoalUnits[i].second > 0)
        {
            y += 10;
            BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x,y), "%d %s", _previousGoalUnits[i].second, _previousGoalUnits[i].first.getName().c_str());
        }
    }
    
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y+15), "Time (ms): %.3lf", _totalPreviousSearchTime);
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y+25), "BO Size: %d", (int)_savedSearchResults.buildOrder.size());
}

// tell the search to keep going for however long we have this frame
void BOSSManager::update(double timeLimit)
{
    // if there's a search in progress, resume it
    if (isSearchInProgress())
    {
        // give the search at least 5ms to search this frame
        double realTimeLimit = timeLimit < 0 ? 5 : timeLimit;
        _smartSearch->setTimeLimit((int)realTimeLimit);

		try
        {
            // call the search to continue searching
            // this will resume a search in progress or start a new search if not yet started
			_smartSearch->search();
		}
        // catch any errors that might happen in the search
		catch (const BOSS::Assert::BOSSException & exception)
        {
            UAB_ASSERT_WARNING(false, "BOSS SmartSearch Exception: %s", exception.what());
			BWAPI::Broodwar->drawTextScreen(0, 0, "Previous search didn't find a solution, resorting to Naive Build Order");
            _previousStatus = "BOSSExeption";

            // if the smart search threw an error, fall back on trying a naive search (less intelligent but faster)
			BOSS::NaiveBuildOrderSearch nbos(_smartSearch->getParameters().initialState, _smartSearch->getParameters().goal);

			try
            {
				_previousBuildOrder = nbos.solve();
				return;
			}
            // if the naive build order search failed then we're screwed, return an empty build order
			catch (const BOSS::Assert::BOSSException & exception)
            {
                UAB_ASSERT_WARNING(false, "BOSS Naive Search Exception: %s", exception.what());
                _previousStatus = "NaiveSearch Exception";

                BWAPI::Broodwar->drawTextScreen(0, 10, "Naive build order search failed");
				BWAPI::Broodwar->printf("No legal BuildOrder found, returning empty Build Order");
				_previousBuildOrder = BOSS::BuildOrder();
				return;
			}
		}

        _totalPreviousSearchTime += _smartSearch->getResults().timeElapsed;

        // after the search finishes for this frame, check to see if we have a solution or if we hit the overall time limit
        if ((BWAPI::Broodwar->getFrameCount() > (_previousSearchStartFrame + SEARCH_FRAME_LIMIT)) || _smartSearch->getResults().solved)
        {
            bool solved = _smartSearch->getResults().solved;

            // if we've found a solution, let us know
            if (_smartSearch->getResults().solved && Config::Debug::DrawBuildOrderSearchInfo)
            {
                //BWAPI::Broodwar->printf("Build order SOLVED in %d nodes", (int)_smartSearch->getResults().nodesExpanded);
            }

            _previousStatus = "BOSS Solved";

            // re-set all the search information to get read for the next search
            _searchInProgress = false;
            _previousSearchFinishFrame = BWAPI::Broodwar->getFrameCount();
            _previousSearchResults = _smartSearch->getResults();
            _savedSearchResults = _previousSearchResults;
            _previousBuildOrder = _previousSearchResults.buildOrder;

            if (solved && _previousBuildOrder.size() == 0)
            {
                _previousStatus = "Trivial Solve";
            }

            // if our search resulted in a build order of size 0 then something failed
            if (!solved && _previousBuildOrder.size() == 0)
            {
                if (Config::Debug::DrawBuildOrderSearchInfo)
                {
                    BWAPI::Broodwar->printf("Previous search didn't find a solution, resorting to Naive Build Order");
                }

                _previousStatus = "No Solution Found";

                // so try another naive build order search as a last resort
                BOSS::NaiveBuildOrderSearch nbos(_smartSearch->getParameters().initialState, _smartSearch->getParameters().goal);

				try
                {
					_previousBuildOrder = nbos.solve();
					return;
				}
                // and if that search doesn't work then we're out of luck, no build orders forus
				catch (const BOSS::Assert::BOSSException & exception)
                {
                    UAB_ASSERT_WARNING(false, "BOSS Timeout Naive Search Exception: %s", exception.what());
                    _previousStatus = "2nd Naive Exception";
                    if (Config::Debug::DrawBuildOrderSearchInfo)
                    {
					    BWAPI::Broodwar->drawTextScreen(0, 20, "No legal BuildOrder found, returning empty Build Order");
                    }
					_previousBuildOrder = BOSS::BuildOrder();
					return;
				}
            }
        }
    }
}

BOSS::BuildOrderSearchGoal BOSSManager::GetGoal(const std::vector<MetaPair> & goalUnits)
{
	BOSS::BuildOrderSearchGoal goal(BOSS::Races::GetRaceID(BWAPI::Broodwar->self()->getRace()));

	for (size_t i=0; i<goalUnits.size(); ++i)
	{
		goal.setGoal(GetActionType(goalUnits[i].first), goalUnits[i].second);
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
    BWAPI::Race r = BWAPI::Broodwar->self()->getRace();
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
    return BuildOrder(BWAPI::Broodwar->self()->getRace(), GetMetaVector(_previousBuildOrder));
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


std::vector<MetaType> BOSSManager::GetNaiveBuildOrder(const std::vector<MetaPair> & goalUnits)
{
    BOSS::GameState                     initialState(BWAPI::Broodwar, BWAPI::Broodwar->self(), BuildingManager::Instance().buildingsQueued());
    BOSS::BuildOrderSearchGoal          goal = GetGoal(goalUnits);

    BOSS::NaiveBuildOrderSearch         nbos(initialState, goal);
    BOSS::BuildOrder                    buildOrder = nbos.solve();

    return GetMetaVector(buildOrder);
}
	
std::vector<MetaType> BOSSManager::GetOptimizedNaiveBuildOrder(const std::vector<MetaPair> & goalUnits)
{
    BOSS::GameState                     initialState(BWAPI::Broodwar, BWAPI::Broodwar->self(), BuildingManager::Instance().buildingsQueued());
    BOSS::BuildOrderSearchGoal          goal = GetGoal(goalUnits);

    BOSS::BuildOrder                    buildOrder = BOSS::Tools::GetOptimizedNaiveBuildOrder(initialState, goal);

    return GetMetaVector(buildOrder);
}
	
			