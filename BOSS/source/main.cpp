#include "BOSS.h"

using namespace BOSS;

GameState getInitialState(const RaceID race)
{
    GameState initialState(race);
    initialState.setStartingState();

    if (false)//race == Races::Protoss)
    {
        initialState.doAction(ActionTypes::Protoss_Probe);
        initialState.doAction(ActionTypes::Protoss_Probe);
        initialState.doAction(ActionTypes::Protoss_Probe);
        initialState.doAction(ActionTypes::Protoss_Probe);
        initialState.doAction(ActionTypes::Protoss_Pylon);
    }

    return initialState;
}

ActionSet getRelevantActions(const RaceID race)
{
    ActionSet actions;

    if (race == Races::Protoss)
    {
        actions.add(ActionTypes::Protoss_Probe);
        actions.add(ActionTypes::Protoss_Pylon);
        actions.add(ActionTypes::Protoss_Nexus);
        actions.add(ActionTypes::Protoss_Assimilator);
        actions.add(ActionTypes::Protoss_Zealot);
        actions.add(ActionTypes::Protoss_Gateway);
        actions.add(ActionTypes::Protoss_Dragoon);
        actions.add(ActionTypes::Protoss_Cybernetics_Core);
    }

    return actions;
}

CombatSearchParameters getParams(const RaceID race)
{
    CombatSearchParameters params;

    if (race == Races::Protoss)
    {
        params.setMaxActions(ActionTypes::Protoss_Probe, 12);
        params.setMaxActions(ActionTypes::Protoss_Nexus, 1);
        params.setMaxActions(ActionTypes::Protoss_Assimilator, 1);
        params.setMaxActions(ActionTypes::Protoss_Cybernetics_Core, 1);
        params.setMaxActions(ActionTypes::Protoss_Pylon, 4);
        params.setMaxActions(ActionTypes::Protoss_Gateway, 3);

        params.setRelevantActions(getRelevantActions(Races::Protoss));
    }

    return params;
}

void testCombatSearch()
{
    RaceID race = Races::Protoss;
    CombatSearchParameters params = getParams(race);
    params.setInitialState(getInitialState(race));
    params.setFrameTimeLimit(3600);

    //params.setSearchTimeLimit(100);
    
    Timer t;
    t.start();

    CombatSearch tcs(params);
    tcs.search();

    double ms = t.getElapsedTimeInMilliSec();
    std::cout << "\nNormal " << tcs.getResults().nodesExpanded << " nodes searched in " << ms << "ms (" << (unsigned long long)(tcs.getResults().nodesExpanded / ms) << "k nodes/s)\n\n";

    for (size_t a(0); a<tcs.getResults().buildOrder.size(); ++a)
    {
        std::cout << tcs.getResults().buildOrder[a].getName() << "\n";
    }
}

void testNaiveBuildOrder()
{
    GameState initialState(Races::Protoss);
    initialState.setStartingState();
    GameState initialStateNew(initialState);
    DFBB_BuildOrderSearchGoal goal(initialState.getRace());

    goal.setGoal(ActionTypes::Protoss_Zealot, 3);
    //goal.setGoal(ActionTypes::Protoss_Dark_Templar, 2);
    //goal.setGoal(ActionTypes::Protoss_Reaver, 1);

    Timer t;
    t.start();
    std::vector<ActionType> buildOrder = Tools::GetNaiveBuildOrder(initialStateNew, goal);
    double ms = t.getElapsedTimeInMilliSec();

    Timer tnew;
    tnew.start();
    std::vector<ActionType> buildOrderNew = Tools::GetOptimizedNaiveBuildOrder(initialStateNew, goal);
    double msnew = tnew.getElapsedTimeInMilliSec();

    for (size_t i(0); i<buildOrder.size(); ++i)
    {
        std::cout << buildOrder[i].getName() << std::endl;
        initialState.doAction(buildOrder[i]);
    }

    std::cout << "Build order took " << initialState.getLastActionFinishTime() << " frames to complete in " << ms << " ms\n\n" << std::endl;

    for (size_t i(0); i<buildOrderNew.size(); ++i)
    {
        std::cout << buildOrderNew[i].getName() << std::endl;
        initialStateNew.doAction(buildOrderNew[i]);
    }

    std::cout << "New build order took " << initialStateNew.getLastActionFinishTime() << " frames to complete with " << initialStateNew.getUnitData().getNumTotal(ActionTypes::GetWorker(initialStateNew.getRace())) << " total workers in " << msnew << " ms" << std::endl;
}

int main()
{
    BOSS::init();

    GameState initialState(Races::Protoss);
    initialState.setStartingState();

    //testNaiveBuildOrder();

    DFBB_BuildOrderSmartSearch smartSearch(initialState.getRace());
    smartSearch.setState(initialState);
    smartSearch.addGoal(ActionTypes::Protoss_Zealot, 6);
    smartSearch.getParameters().goal.printGoal();
    smartSearch.setTimeLimit(50000);
    
    Timer t;
    t.start();
    while (!smartSearch.getResults().solved)
    {
        smartSearch.search();
    }

    double ms = t.getElapsedTimeInMilliSec();

    smartSearch.getResults().printResults(true);
    std::cout << std::endl << "Search Took: " << ms << "ms" << std::endl;

    //Tools::TestBuildOrderUpperBound();

    //testCombatSearch();
    //testThreadCombatSearch();

    return 0;
}

