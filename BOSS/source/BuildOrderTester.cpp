#include "BuildOrderTester.h"
#include "JSONTools.h"
#include "NaiveBuildOrderSearch.h"

using namespace BOSS;

BuildOrderSearchGoal BuildOrderTester::GetRandomGoal(const RaceID race)
{
    BuildOrderSearchGoal goal(race);
    
    const std::vector<ActionType> & allActionTypes = ActionTypes::GetAllActionTypes(race);
    size_t totalActionTypes = allActionTypes.size();

    size_t numUnits = 6;
    size_t maxOfUnit = 5;

    for (size_t i(0); i < numUnits; ++i)
    {
        const ActionType randomAction = allActionTypes[rand() % totalActionTypes];

        if (randomAction.getUnitType() == BWAPI::UnitTypes::Protoss_Dark_Archon ||
            randomAction.getUnitType() == BWAPI::UnitTypes::Protoss_Archon ||
            randomAction.getUnitType() == BWAPI::UnitTypes::Zerg_Larva)
        {
            continue;
        }

        int numToAdd = rand() % maxOfUnit;

        if (randomAction.isUnit() && !randomAction.isRefinery())
        {
            
            goal.setGoal(randomAction, numToAdd);
        }
        else
        {
            goal.setGoal(randomAction, 1);
        }
    }

    return goal;
}

void BuildOrderTester::TestRandomBuilds(const RaceID race, const size_t numTests)
{
    for (size_t i(0); i < numTests; ++i)
    {
        GameState state = GetStartState(race, 20);            
    }
}

GameState BuildOrderTester::GetStartState(const RaceID race, int randomActions)
{
    GameState state(race);
    state.setStartingState();
    GameState copyState(state);
    
    BuildOrder randomBuildOrder;
    int i = 0;

    //try
    {
        for (i = 0; i < randomActions; ++i)
        {
            ActionSet legalActions;
            state.getAllLegalActions(legalActions);
            
            if (legalActions.isEmpty())
            {
                std::cout << i << " Legal Actions Empty!" << std::endl;
                std::cout << randomBuildOrder.getNumberedString() << std::endl;
                std::cout << state.toString() << std::endl;
               
            }


            ActionType randomAction = legalActions[rand() % legalActions.size()];

           

            if (!randomAction.isResourceDepot())
            {
                BOSS_ASSERT(state.isLegal(randomAction), "Should be a legal action!");

                randomBuildOrder.add(randomAction);
                state.doAction(randomAction);
            }
        }
    }
    /*catch (Assert::BOSSException e)
    {
        std::cout << std::endl << "Random State Error @ " << i << ": " << randomBuildOrder[i-1].getName() << std::endl << std::endl;

        std::cout << randomBuildOrder.getJSONString() << std::endl;

        std::cout << randomBuildOrder.getNumberedString() << std::endl;

        std::cout << state.toString() << std::endl;

        for (size_t a(0); a < randomBuildOrder.size(); ++a)
        {
            std::cout << "About to do: " << randomBuildOrder[a].getName() << std::endl;
            std::cout << copyState.toString() << std::endl;
            copyState.doAction(randomBuildOrder[a]);
        }

        randomBuildOrder.doActions(copyState, 0, randomBuildOrder.size()-1);

        std::cout << copyState.toString();

        copyState.doAction(randomBuildOrder[randomBuildOrder.size()-1]);

        exit(-1);
    }*/

    return state;
}

void BuildOrderTester::DoRandomTests(const RaceID race, const size_t numTests)
{
    GameState startState(race);
    startState.setStartingState();
    
    srand((int)time(NULL));

    for (size_t i(0); i < numTests; ++i)
    {
        if (i && !(i % 100000))
        {
            std::cout << Races::GetRaceName(race) << " " << i << std::endl;
        }

        //GameState startState = GetStartState(race, 20);
        BuildOrderSearchGoal goal = GetRandomGoal(race);
        
        NaiveBuildOrderSearch naiveSearch(startState, goal);
        
        const BuildOrder & buildOrder = naiveSearch.solve();

        GameState currentState(startState);
        bool buildOrderIsLegal = buildOrder.doActions(currentState);

        if (buildOrderIsLegal)
        {
            if (goal.isAchievedBy(currentState))
            {
                //std::cout << i << "  Test Passed!" << std::endl;
            }
            else
            {
                goal.printGoal();
                for (size_t a(0); a < buildOrder.size(); ++a)
                {
                    std::cout << a << " " << buildOrder[a].getName() << std::endl;
                }
                std::cout << i << "  Found build order did not meet goal" << std::endl;
                std::cout << buildOrder.getJSONString() << std::endl;
                std::cout << currentState.toString() << std::endl;
                goal.printGoal();
            }
        }
        else
        {
            goal.printGoal();
            for (size_t a(0); a < buildOrder.size(); ++a)
            {
                std::cout << a << " " << buildOrder[a].getName() << std::endl;
            }
            std::cout << buildOrder.getJSONString() << std::endl;
            std::cout << i << "  Found build-order was not legal" << std::endl;
            std::cout << currentState.toString() << std::endl;

        }
    }
}