#include "ThreadCombatSearch.h"

using namespace BOSS;

ThreadCombatSearch::ThreadCombatSearch(const CombatSearchParameters p)
    : _params(p)
{
    
    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}

std::vector<CombatSearch>   _searches;
std::vector<std::thread>    _threads;

void ThreadCombatSearch::search()
{
    //_threads.reserve(10);

    ActionSet legalActions;
    CombatSearch::generateLegalActions(_params.getInitialState(), legalActions, _params);

    for (size_t a(0); a < legalActions.size(); ++a)
    {
        GameState child(_params.getInitialState());
        child.doAction(legalActions[a]);
        //std::cout << "Legal Action: " << legalActions[a].getName() << "\n";

        CombatSearchParameters p(_params);
        p.setInitialState(child);

        _searches.push_back(CombatSearch(p));
        //_searches[a].search();
        //_threads.push_back(std::thread(&ThreadCombatSearch::launchThread, this, a));
    }

    for (size_t s(0); s < _searches.size(); ++s)
    {
        _threads.push_back(std::thread(&ThreadCombatSearch::launchThread, this, s));
    }

    for (size_t t(0); t < _threads.size(); ++t)
    {
        _threads[t].join();
    }

    for (size_t s(0); s < _searches.size(); ++s)
    {
        _results.nodesExpanded += _searches[s].getResults().nodesExpanded;
    }
}

void ThreadCombatSearch::launchThread(size_t threadNum)
{
    _searches[threadNum].search();

    //std::cout << threadNum << " " << _searches[threadNum].getResults().nodesExpanded << "\n";
}

const CombatSearchResults & ThreadCombatSearch::getResults() const
{
    return _results;
}
