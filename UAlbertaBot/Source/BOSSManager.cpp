#include "Common.h"
#include "BOSSManager.h"
#include "UnitUtil.h"
#include "Global.h"
#include "ProductionManager.h"
#include "WorkerManager.h"
#include "StrategyManager.h"
#include "Logger.h"

using namespace UAlbertaBot;

// constructor
BOSSManager::BOSSManager() 
{
	
}

void BOSSManager::reset()
{
    m_previousSearchResults = BOSS::DFBB_BuildOrderSearchResults();
    m_searchInProgress = false;
    m_previousBuildOrder.clear();
}

// start a new search for a new goal
void BOSSManager::startNewSearch(const std::vector<MetaPair> & goalUnits)
{
    size_t numWorkers   = UnitUtil::GetAllUnitCount(BWAPI::Broodwar->self()->getRace().getWorker());
    size_t numDepots    = UnitUtil::GetAllUnitCount(BWAPI::Broodwar->self()->getRace().getResourceDepot())
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Lair)
                        + UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hive);

    if (numWorkers == 0)
    {
        m_previousStatus = "\x08No Workers :(";
        return;
    }

    if (numDepots == 0)
    {
        m_previousStatus = "\x08No Depots :(";
        return;
    }

    // convert from UAlbertaBot's meta goal type to BOSS ActionType goal
    try
    {
        BOSS::BuildOrderSearchGoal goal = GetGoal(goalUnits);

        BOSS::GameState initialState(BWAPI::Broodwar, BWAPI::Broodwar->self(), Global::Production().buildingsQueued());

        m_smartSearch = SearchPtr(new BOSS::DFBB_BuildOrderSmartSearch(initialState.getRace()));
        m_smartSearch->setGoal(GetGoal(goalUnits));
        m_smartSearch->setState(initialState);

        m_searchInProgress = true;
        m_previousSearchStartFrame = BWAPI::Broodwar->getFrameCount();
        m_totalPreviousSearchTime = 0;
        m_previousGoalUnits = goalUnits;
    }
    catch (const BOSS::BOSSException)
    {
        BWAPI::BroodwarPtr->printf("Exception in BOSS::GameState constructor, will try again next frame");
    }
}

void BOSSManager::drawSearchInformation(int x, int y) 
{
	if (!Config::Debug::DrawBuildOrderSearchInfo)
    {
        return;
    }

    // draw the background
    int width = 155;
    int height = 80;
    BWAPI::Broodwar->drawBoxScreen(BWAPI::Position(x-5,y), BWAPI::Position(x+width, y+height), BWAPI::Colors::Black, true);

    x += 5; y+=3;

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y), "%cBuildOrderSearch:", '\x04');
    y += 10;
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y), "%s", m_previousStatus.c_str());

    for (size_t i(0); i < m_previousGoalUnits.size(); ++i)
    {
        if (m_previousGoalUnits[i].second > 0)
        {
            y += 10;
            BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x,y), "%d %s", m_previousGoalUnits[i].second, m_previousGoalUnits[i].first.getName().c_str());
        }
    }
    
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y+25), "Time (ms): %.3lf", m_totalPreviousSearchTime);
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y+35), "Nodes: %d", m_savedSearchResults.nodesExpanded);
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y+45), "BO Size: %d", (int)m_savedSearchResults.buildOrder.size());
}

void BOSSManager::drawStateInformation(int x, int y) 
{
	if (!Config::Debug::DrawBOSSStateInfo)
    {
        return;
    }

    BOSS::GameState currentState(BWAPI::Broodwar, BWAPI::Broodwar->self(), Global::Production().buildingsQueued());
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x-100, y+30), "\x04%s", currentState.getBuildingData().toString().c_str());
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x+150, y), "\x04%s", currentState.toString().c_str());
    
}

// tell the search to keep going for however long we have this frame
void BOSSManager::update(double timeLimit)
{
    PROFILE_FUNCTION();

    // if there's a search in progress, resume it
    if (isSearchInProgress())
    {
        m_previousStatus.clear();

        // give the search at least 5ms to search this frame
        double realTimeLimit = timeLimit < 0 ? 5 : timeLimit;
        m_smartSearch->setTimeLimit((int)realTimeLimit);
        bool caughtException = false;

		try
        {
            // call the search to continue searching
            // this will resume a search in progress or start a new search if not yet started
			m_smartSearch->search();
		}
        // catch any errors that might happen in the search
		catch (const BOSS::BOSSException & exception)
        {
            UAB_ASSERT_WARNING(false, "BOSS SmartSearch Exception: %s", exception.what());
			BWAPI::Broodwar->drawTextScreen(0, 0, "Previous search didn't find a solution, resorting to Naive Build Order");
            m_previousStatus = "BOSSExeption";
            caughtException = true;
		}

        m_totalPreviousSearchTime += m_smartSearch->getResults().timeElapsed;

        // after the search finishes for this frame, check to see if we have a solution or if we hit the overall time limit
        bool searchTimeOut = (BWAPI::Broodwar->getFrameCount() > (m_previousSearchStartFrame + Config::Macro::BOSSFrameLimit));
        bool previousSearchComplete = searchTimeOut || m_smartSearch->getResults().solved || caughtException;
        if (previousSearchComplete)
        {
            bool solved = m_smartSearch->getResults().solved && m_smartSearch->getResults().solutionFound;

            // if we've found a solution, let us know
            if (m_smartSearch->getResults().solved && Config::Debug::DrawBuildOrderSearchInfo)
            {
                //BWAPI::Broodwar->printf("Build order SOLVED in %d nodes", (int)_smartSearch->getResults().nodesExpanded);
            }

            if (m_smartSearch->getResults().solved)
            {
                if (m_smartSearch->getResults().solutionFound)
                {
                    m_previousStatus = std::string("\x07") + "BOSS Solve Solution\n";
                }
                else
                {
                    m_previousStatus = std::string("\x03") + "BOSS Solve NoSolution\n";
                }
            }

            // re-set all the search information to get read for the next search
            m_searchInProgress = false;
            m_previousSearchFinishFrame = BWAPI::Broodwar->getFrameCount();
            m_previousSearchResults = m_smartSearch->getResults();
            m_savedSearchResults = m_previousSearchResults;
            m_previousBuildOrder = m_previousSearchResults.buildOrder;

            if (solved && m_previousBuildOrder.size() == 0)
            {
                m_previousStatus = std::string("\x07") + "BOSS Trivial Solve\n";
            }

            // if our search resulted in a build order of size 0 then something failed
            if (!solved && m_previousBuildOrder.size() == 0)
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
                BOSS::NaiveBuildOrderSearch nbos(m_smartSearch->getParameters().initialState, m_smartSearch->getParameters().goal);

				try
                {
                    if (searchTimeOut)
                    {
                        m_previousStatus = std::string("\x02") + "BOSS Timeout\n";
                    }

                    if (caughtException)
                    {
                        m_previousStatus = std::string("\x02") + "BOSS Exception\n";
                    }

					m_previousBuildOrder = nbos.solve();
                    m_previousStatus += "\x03NBOS Solution";

					return;
				}
                // and if that search doesn't work then we're out of luck, no build orders forus
				catch (const BOSS::BOSSException & exception)
                {
                    UAB_ASSERT_WARNING(false, "BOSS Timeout Naive Search Exception: %s", exception.what());
                    m_previousStatus += "\x08Naive Exception";
                    if (Config::Debug::DrawBuildOrderSearchInfo)
                    {
					    BWAPI::Broodwar->drawTextScreen(0, 20, "No legal BuildOrder found, returning empty Build Order");
                    }
					m_previousBuildOrder = BOSS::BuildOrder();
					return;
				}
            }
        }
    }
}

void BOSSManager::logBadSearch()
{
    std::string s = m_smartSearch->getParameters().toString();

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
    return m_searchInProgress;
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
    return BuildOrder(BWAPI::Broodwar->self()->getRace(), GetMetaVector(m_previousBuildOrder));
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
