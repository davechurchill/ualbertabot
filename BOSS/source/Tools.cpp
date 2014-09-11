#include "Tools.h"
#include "DFBB_BuildOrderSearchGoal.h"

using namespace BOSS;

std::vector<ActionType> Tools::GetNaiveBuildOrder(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    PrerequisiteSet wanted;
    int minWorkers = 8;

    // add everything from the goal to the needed set
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        UnitCountType numCompleted = state.getUnitData().getNumTotal(actionType);
            
        if (goal.getGoal(actionType) > numCompleted)
        {
            wanted.addUnique(actionType);
        }
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    std::vector<ActionType> buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        buildOrder.push_back(requiredToBuild.getActionType(a));
    }

    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
    int requiredWorkers = minWorkers - state.getUnitData().getNumCompleted(ActionTypes::GetWorker(state.getRace()));
    while (requiredWorkers-- > 0)
    {
        buildOrder.push_back(ActionTypes::GetWorker(state.getRace()));
    }

    // Add the goal units to the end of the build order 
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        int numNeeded = (int)goal.getGoal(actionType) - (int)state.getUnitData().getNumTotal(actionType);
            
        while (numNeeded-- > 1)
        {
            buildOrder.push_back(actionType);
        }
    }

    // Check to see if we have enough buildings for the required addons
    if (state.getRace() == Races::Terran)
    {
        int commandCenterAddons = 0;
        int factoryAddons = 0;
        int starportAddons = 0;
        int sciAddons = 0;
        int numCommandCenters = state.getUnitData().getNumTotal(ActionTypes::Terran_Command_Center);
        int numFactories = state.getUnitData().getNumTotal(ActionTypes::Terran_Factory);
        int numStarports = state.getUnitData().getNumTotal(ActionTypes::Terran_Starport);
        int numSci = state.getUnitData().getNumTotal(ActionTypes::Terran_Science_Facility);
        
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            const ActionType & actionType = buildOrder[a];

            if (actionType.isAddon())
            {
                if (actionType.whatBuildsActionType() == ActionTypes::Terran_Command_Center)
                {
                    ++commandCenterAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Factory)
                {
                    ++factoryAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Starport)
                {
                    ++starportAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Science_Facility)
                {
                    ++sciAddons;
                }
                else
                {
                    BOSS_ASSERT(false, "Inconceivable: %s", actionType.getName().c_str());
                }
            }

            if (actionType == ActionTypes::Terran_Command_Center)
            {
                ++numCommandCenters;
            }
            else if (actionType == ActionTypes::Terran_Factory)
            {
                ++numFactories;
            }
            else if (actionType == ActionTypes::Terran_Starport)
            {
                ++numStarports;
            }
            else if (actionType == ActionTypes::Terran_Science_Facility)
            {
                ++numSci;
            }
        }

        // add the necessary buildings to make the addons
        for (int n(0); n < commandCenterAddons - numCommandCenters; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Command_Center);
        }

        for (int n(0); n < factoryAddons - numFactories; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Factory);
        }

        for (int n(0); n < starportAddons - numStarports; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Starport);
        }
        for (int n(0); n < sciAddons - numSci; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Science_Facility);
        }

    }

    // Bubble sort the build order so that prerequites always come before what requires them
    for (size_t i(0); i < buildOrder.size()-1; ++i)
    {
        for (size_t j(i+1); j < buildOrder.size(); ++j)
        {
            const PrerequisiteSet & recursivePre = buildOrder[i].getRecursivePrerequisites();

            if (recursivePre.contains(buildOrder[j]))
            {
                std::swap(buildOrder[i], buildOrder[j]);
            }
        }
    }

    // Insert supply buildings so that build order is legal w.r.t. supply counts
    int maxSupply = state.getUnitData().getMaxSupply() + state.getUnitData().getSupplyInProgress();
    int currentSupply = state.getUnitData().getCurrentSupply();

    std::vector<ActionType> finalBuildOrder;
    for (size_t a(0); a < buildOrder.size(); ++a)
    {
        int surplusSupply = maxSupply - currentSupply;
        if (surplusSupply < buildOrder[a].supplyRequired())
        {
            const ActionType & supplyProvider = ActionTypes::GetSupplyProvider(state.getRace());
            finalBuildOrder.push_back(supplyProvider);
            maxSupply += supplyProvider.supplyProvided();
        }

        finalBuildOrder.push_back(buildOrder[a]);
        currentSupply += buildOrder[a].supplyRequired();
        maxSupply += buildOrder[a].supplyProvided();
    }

    // Print it out for debugging
    /*std::cout << std::endl;
    for (size_t a(0); a < finalBuildOrder.size(); ++a)
    {
        std::cout << a << "   " << finalBuildOrder[a].getName() << std::endl;
    }*/

    return finalBuildOrder;
}

std::vector<ActionType> Tools::GetOptimizedNaiveBuildOrder(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    std::vector<ActionType> bestBuildOrder = GetNaiveBuildOrderNew(state, goal, 4);
    FrameCountType minCompletionTime = Tools::GetBuildOrderCompletionTime(state, bestBuildOrder);
    UnitCountType bestNumWorkers = GetWorkerCount(bestBuildOrder);

    for (UnitCountType numWorkers(4); numWorkers < 27; ++numWorkers)
    {
        std::vector<ActionType> buildOrder = Tools::GetNaiveBuildOrderNew(state, goal, numWorkers);
        FrameCountType completionTime = Tools::GetBuildOrderCompletionTime(state, buildOrder);
        UnitCountType workers = GetWorkerCount(buildOrder);
        
        if (completionTime <= minCompletionTime + ((workers-bestNumWorkers)*24))
        {
            std::cout << numWorkers << " " << workers << " " << completionTime << std::endl;

            minCompletionTime = completionTime;
            bestBuildOrder = buildOrder;
        }
    }

    return bestBuildOrder;
}

std::vector<ActionType> Tools::GetNaiveBuildOrderNew(const GameState & state, const DFBB_BuildOrderSearchGoal & goal, UnitCountType maxWorkers)
{
    PrerequisiteSet wanted;
    int minWorkers = 8;

    // add everything from the goal to the needed set
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        UnitCountType numCompleted = state.getUnitData().getNumTotal(actionType);
            
        if (goal.getGoal(actionType) > numCompleted)
        {
            wanted.addUnique(actionType);
        }
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    std::vector<ActionType> buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        buildOrder.push_back(requiredToBuild.getActionType(a));
    }

    // Add the goal units to the end of the build order 
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        int numNeeded = (int)goal.getGoal(actionType) - (int)state.getUnitData().getNumTotal(actionType);
            
        while (numNeeded-- > 1)
        {
            buildOrder.push_back(actionType);
        }
    }

    // Check to see if we have enough buildings for the required addons
    if (state.getRace() == Races::Terran)
    {
        int commandCenterAddons = 0;
        int factoryAddons = 0;
        int starportAddons = 0;
        int sciAddons = 0;
        int numCommandCenters = state.getUnitData().getNumTotal(ActionTypes::Terran_Command_Center);
        int numFactories = state.getUnitData().getNumTotal(ActionTypes::Terran_Factory);
        int numStarports = state.getUnitData().getNumTotal(ActionTypes::Terran_Starport);
        int numSci = state.getUnitData().getNumTotal(ActionTypes::Terran_Science_Facility);
        
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            const ActionType & actionType = buildOrder[a];

            if (actionType.isAddon())
            {
                if (actionType.whatBuildsActionType() == ActionTypes::Terran_Command_Center)
                {
                    ++commandCenterAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Factory)
                {
                    ++factoryAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Starport)
                {
                    ++starportAddons;
                }
                else if (actionType.whatBuildsActionType() == ActionTypes::Terran_Science_Facility)
                {
                    ++sciAddons;
                }
                else
                {
                    BOSS_ASSERT(false, "Inconceivable: %s", actionType.getName().c_str());
                }
            }

            if (actionType == ActionTypes::Terran_Command_Center)
            {
                ++numCommandCenters;
            }
            else if (actionType == ActionTypes::Terran_Factory)
            {
                ++numFactories;
            }
            else if (actionType == ActionTypes::Terran_Starport)
            {
                ++numStarports;
            }
            else if (actionType == ActionTypes::Terran_Science_Facility)
            {
                ++numSci;
            }
        }

        // add the necessary buildings to make the addons
        for (int n(0); n < commandCenterAddons - numCommandCenters; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Command_Center);
        }

        for (int n(0); n < factoryAddons - numFactories; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Factory);
        }

        for (int n(0); n < starportAddons - numStarports; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Starport);
        }
        for (int n(0); n < sciAddons - numSci; ++n)
        {
            buildOrder.push_back(ActionTypes::Terran_Science_Facility);
        }

    }

    // Bubble sort the build order so that prerequites always come before what requires them
    for (size_t i(0); i < buildOrder.size()-1; ++i)
    {
        for (size_t j(i+1); j < buildOrder.size(); ++j)
        {
            const PrerequisiteSet & recursivePre = buildOrder[i].getRecursivePrerequisites();

            if (recursivePre.contains(buildOrder[j]))
            {
                std::swap(buildOrder[i], buildOrder[j]);
            }
        }
    }

    // finish the build order with workers and supply
    std::vector<ActionType> finalBuildOrder;
    GameState currentState(state);
    size_t i = 0;
    while (i < buildOrder.size())
    {
        const ActionType & worker           = ActionTypes::GetWorker(currentState.getRace());
        const ActionType & supplyProvider   = ActionTypes::GetSupplyProvider(currentState.getRace());
        const ActionType & nextAction       = buildOrder[i];
        UnitCountType maxSupply             = currentState.getUnitData().getMaxSupply() + currentState.getUnitData().getSupplyInProgress();
        UnitCountType numWorkers            = currentState.getUnitData().getNumTotal(worker);
        UnitCountType currentSupply         = currentState.getUnitData().getCurrentSupply();

        if (numWorkers < 8)
        {
            finalBuildOrder.push_back(worker);
            currentState.doAction(worker);
            continue;
        }

        // insert a supply provider if we are behind
        int surplusSupply = maxSupply - currentSupply;
        if (surplusSupply < nextAction.supplyRequired() + 1)
        {
            BOSS_ASSERT(currentState.isLegal(supplyProvider), "supplyProvider should be legal");
            finalBuildOrder.push_back(supplyProvider);
            currentState.doAction(supplyProvider);
            continue;
        }

        // check to see if we should insert a worker
        BOSS_ASSERT(currentState.isLegal(worker), "Worker should be legal");
        BOSS_ASSERT(currentState.isLegal(nextAction), "nextAction should be legal");

        FrameCountType whenWorkerReady      = currentState.whenCanPerform(worker);
        FrameCountType whennextActionReady  = currentState.whenCanPerform(nextAction);

        if ((numWorkers < maxWorkers) && (whenWorkerReady <= whennextActionReady))
        {
            finalBuildOrder.push_back(worker);
            currentState.doAction(worker);
            continue;
        }
        else
        {
            finalBuildOrder.push_back(nextAction);
            currentState.doAction(nextAction);
            ++i;
        }
    }

    FrameCountType bestCompletionTime = Tools::GetBuildOrderCompletionTime(state, finalBuildOrder);
    //std::cout << "Initial build order takes " << bestCompletionTime << " frames\n\n";
    //std::cout << "BO Size: " << finalBuildOrder.size() << std::endl;

    while (true)
    {
        std::vector<ActionType> testBuildOrder = InsertActionIntoBuildOrder(finalBuildOrder, state, ActionTypes::Protoss_Gateway);

        FrameCountType completionTime = Tools::GetBuildOrderCompletionTime(state, testBuildOrder);

        if (completionTime < bestCompletionTime)
        {
            //std::cout << "Found a better build order that takes " << completionTime << " frames\n";
            bestCompletionTime = completionTime;
            finalBuildOrder = testBuildOrder;
        }
        else
        {
            break;
        }
    }

    return finalBuildOrder;
}

UnitCountType Tools::GetWorkerCount(const std::vector<ActionType> & buildOrder)
{
    UnitCountType count = 0;
    for (size_t a(0); a < buildOrder.size(); ++a)
    {
        if (buildOrder[a].isWorker())
        {
            ++count;
        }
    }

    return count;
}

std::vector<ActionType> Tools::InsertActionIntoBuildOrder(const std::vector<ActionType> & buildOrder, const GameState & initialState, const ActionType & action)
{
    std::vector<ActionType> bestBuildOrder = buildOrder;
    std::vector<ActionType> runningBuildOrder;
    GameState runningState(initialState);
    FrameCountType minCompletionTime = Tools::GetBuildOrderCompletionTime(initialState, buildOrder);

    for (size_t insertIndex(0); insertIndex < buildOrder.size(); ++insertIndex)
    {
        // if we can test the action here, do it
        if (runningState.isLegal(action))
        {
            std::vector<ActionType> testBuildOrder(runningBuildOrder);
            testBuildOrder.push_back(action);
            for (size_t rIndex(insertIndex); rIndex < buildOrder.size(); ++rIndex)
            {
                testBuildOrder.push_back(buildOrder[rIndex]);
            }

            FrameCountType completionTime = Tools::GetBuildOrderCompletionTime(initialState, testBuildOrder);

            if (completionTime < minCompletionTime)
            {
                minCompletionTime = completionTime;
                bestBuildOrder = testBuildOrder;
                
            }
        }

        runningBuildOrder.push_back(buildOrder[insertIndex]);

        BOSS_ASSERT(runningState.isLegal(buildOrder[insertIndex]), "We have made the next action illegal somehow");

        runningState.doAction(buildOrder[insertIndex]);
    }

    return bestBuildOrder;
}

FrameCountType Tools::GetBuildOrderCompletionTime(const GameState & state, const std::vector<ActionType> & buildOrder)
{
    GameState currentState(state);

    for (size_t a(0); a < buildOrder.size(); ++a)
    {
        BOSS_ASSERT(currentState.isLegal(buildOrder[a]), "build order item not legal");
        currentState.doAction(buildOrder[a]);
    }

    return currentState.getLastActionFinishTime();
}

FrameCountType Tools::GetUpperBound(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    std::vector<ActionType> naiveBuildOrder = Tools::GetNaiveBuildOrder(state, goal);
        
    FrameCountType upperBound = Tools::GetBuildOrderCompletionTime(state, naiveBuildOrder);

    return upperBound;
}

FrameCountType Tools::GetLowerBound(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    PrerequisiteSet wanted;

    // add everything from the goal to the needed set
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        UnitCountType numCompleted = state.getUnitData().getNumTotal(actionType);
            
        if (goal.getGoal(actionType) > numCompleted)
        {
            wanted.addUnique(actionType);
        }
    }

    FrameCountType lowerBound = Tools::CalculatePrerequisitesLowerBound(state, wanted, 0);

    return lowerBound;
}

void Tools::CalculatePrerequisitesRequiredToBuild(const GameState & state, const PrerequisiteSet & needed, PrerequisiteSet & added)
{
    // if anything needed gas and we don't have a refinery, we need to add one
    PrerequisiteSet allNeeded(needed);
    const ActionType & refinery = ActionTypes::GetRefinery(state.getRace());
    if (!needed.contains(refinery) && (state.getUnitData().getNumCompleted(refinery) == 0) && !added.contains(refinery))
    {
        for (size_t n(0); n<needed.size(); ++n)
        {
            if (needed.getActionType(n).gasPrice() > 0)
            {
                allNeeded.add(refinery);
                break;
            }
        }
    }

    for (size_t n(0); n<allNeeded.size(); ++n)
    {
        const ActionType & neededType = allNeeded.getActionType(n);

        // if we already have the needed type completed we can skip it
        if (added.contains(neededType) || state.getUnitData().getNumCompleted(neededType) > 0)
        {
            
        }
        // if we have the needed type in progress we can add that time
        else if (state.getUnitData().getNumInProgress(neededType) > 0)
        {
            added.add(neededType);
        }
        // otherwise we need to recurse on the needed type to build its prerequisites
        else
        {
            added.add(neededType);
            CalculatePrerequisitesRequiredToBuild(state, neededType.getPrerequisites(), added);
        }
    }
}

// returns the amount of time necessary to complete the longest chain of sequential prerequisites
FrameCountType Tools::CalculatePrerequisitesLowerBound(const GameState & state, const PrerequisiteSet & needed, FrameCountType timeSoFar, int depth)
{
    FrameCountType max = 0;
    for (size_t n(0); n<needed.size(); ++n)
    {
        const ActionType & neededType = needed.getActionType(n);
        FrameCountType thisActionTime = 0;

        // if we already have the needed type completed we can skip it
        if (state.getUnitData().getNumCompleted(neededType) > 0)
        {
            thisActionTime = timeSoFar;
        }
        // if we have the needed type in progress we can add that time
        else if (state.getUnitData().getNumInProgress(neededType) > 0)
        {
            thisActionTime = timeSoFar + state.getUnitData().getFinishTime(neededType) - state.getCurrentFrame();
        }
        // otherwise we need to recurse on the needed type to build its prerequisites
        else
        {
            /*for (int i=0; i<depth; ++i)
            {
                std::cout << "    ";
            }
            std::cout << neededType.getName() << " " << neededType.buildTime() << " " << timeSoFar << std::endl;*/
            thisActionTime = CalculatePrerequisitesLowerBound(state, neededType.getPrerequisites(), timeSoFar + neededType.buildTime(), depth + 1);
        }

        if (thisActionTime > max)
        {
            max = thisActionTime;
        }
    }

    return max;
}

void Tools::TestBuildOrderUpperBound()
{
    GameState initialStates[Races::NUM_RACES];
    for (size_t r(0); r<Races::NUM_RACES; ++r)
    {
        initialStates[r] = GameState(r);
        initialStates[r].setStartingState();
    }
    
    size_t numTests = 10;
    for (size_t test(0); test < numTests; ++test)
    {
        RaceID race = test % 2; // ignore zerg until it works
        DFBB_BuildOrderSearchGoal goal(race);

        if (test % 10000 == 0)
        {
            std::cout << test << std::endl;
        }

        // add 5 random units to the goal
        size_t numRandom = 10;
        for (size_t r(0); r < numRandom; ++r)
        {
            const ActionType & actionType = ActionTypes::GetActionType(race, rand() % ActionTypes::GetAllActionTypes(race).size());

            if (!actionType.isRefinery() && !actionType.isAddon())
            {
                goal.setGoal(actionType, goal.getGoal(actionType) + 1);
            }
        }
        
        goal.printGoal();

        FrameCountType upperBound = Tools::GetUpperBound(initialStates[race], goal);
        std::cout << "\nUpper Bound is: " << upperBound << std::endl;
    }
}

bool Tools::MeetsGoal(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);

        if (state.getUnitData().getNumTotal(actionType) < goal.getGoal(actionType))
        {
            return false;
        }
    }

    return true;
}