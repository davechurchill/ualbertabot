#include "BuildOrderTester.h"
#include "JSONTools.h"

using namespace BOSS;

DFBB_BuildOrderSearchGoal BuildOrderTester::GetRandomGoal(const RaceID race)
{
    DFBB_BuildOrderSearchGoal goal(race);
    
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

void BuildOrderTester::DoRandomTests(const RaceID race, const size_t numTests)
{
    GameState startState(race);
    startState.setStartingState();
    srand(time(NULL));

    for (size_t i(0); i < numTests; ++i)
    {
        // make a pseudo-random goal
        DFBB_BuildOrderSearchGoal goal = GetRandomGoal(race);
        //goal.printGoal();
        
        std::vector<ActionType> buildOrder = Tools::GetNaiveBuildOrder(startState, goal);

        GameState currentState(startState);
        bool buildOrderIsLegal = Tools::PerformBuildOrder(currentState, buildOrder);

        if (buildOrderIsLegal)
        {
            if (Tools::MeetsGoal(currentState, goal))
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
                std::cout << JSONTools::GetBuildOrderString(buildOrder) << std::endl;
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
            std::cout << JSONTools::GetBuildOrderString(buildOrder) << std::endl;
            std::cout << i << "  Found build-order was not legal" << std::endl;
            std::cout << currentState.toString() << std::endl;

        }
    }
}