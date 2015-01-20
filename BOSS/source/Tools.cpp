#include "Tools.h"
#include "DFBB_BuildOrderSearchGoal.h"

using namespace BOSS;

std::vector<ActionType> Tools::GetNaiveBuildOrder(const GameState & state, const DFBB_BuildOrderSearchGoal & goal)
{
    PrerequisiteSet wanted;
    int minWorkers = 8;

    const ActionType & worker = ActionTypes::GetWorker(state.getRace());
    std::vector<size_t> buildOrderActionTypeCount(ActionTypes::GetAllActionTypes(state.getRace()).size(), 0);

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

    if (wanted.size() == 0)
    {
        return std::vector<ActionType>();
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    std::vector<ActionType> buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        const ActionType & type = requiredToBuild.getActionType(a);
        buildOrder.push_back(type);
        buildOrderActionTypeCount[type.ID()]++;
    }

    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
    int requiredWorkers = minWorkers - state.getUnitData().getNumCompleted(ActionTypes::GetWorker(state.getRace()));
    while (requiredWorkers-- > 0)
    {
        buildOrder.push_back(worker);
        buildOrderActionTypeCount[worker.ID()]++;
    }

    // Add the goal units to the end of the build order 
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        int need = (int)goal.getGoal(actionType);
        int have = (int)state.getUnitData().getNumTotal(actionType);
        int numNeeded = need - have - buildOrderActionTypeCount[actionType.ID()]; 
            
        for (int i(0); i < numNeeded; ++i)
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

        static const ActionType commandCenter = ActionTypes::GetActionType("Terran_Command_Center");
        static const ActionType factory = ActionTypes::GetActionType("Terran_Factory");
        static const ActionType starport = ActionTypes::GetActionType("Terran_Starport");
        static const ActionType scienceFacility = ActionTypes::GetActionType("Terran_Science_Facility");

        int numCommandCenters = state.getUnitData().getNumTotal(commandCenter);
        int numFactories = state.getUnitData().getNumTotal(factory);
        int numStarports = state.getUnitData().getNumTotal(starport);
        int numSci = state.getUnitData().getNumTotal(scienceFacility);
        
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            const ActionType & actionType = buildOrder[a];

            if (actionType.isAddon())
            {
                if (actionType.whatBuildsActionType() == commandCenter)
                {
                    ++commandCenterAddons;
                }
                else if (actionType.whatBuildsActionType() == factory)
                {
                    ++factoryAddons;
                }
                else if (actionType.whatBuildsActionType() == starport)
                {
                    ++starportAddons;
                }
                else if (actionType.whatBuildsActionType() == scienceFacility)
                {
                    ++sciAddons;
                }
                else
                {
                    BOSS_ASSERT(false, "Inconceivable: %s", actionType.getName().c_str());
                }
            }

            if (actionType == commandCenter)
            {
                ++numCommandCenters;
            }
            else if (actionType == factory)
            {
                ++numFactories;
            }
            else if (actionType == starport)
            {
                ++numStarports;
            }
            else if (actionType == scienceFacility)
            {
                ++numSci;
            }
        }

        // add the necessary buildings to make the addons
        for (int n(0); n < commandCenterAddons - numCommandCenters; ++n)
        {
            buildOrder.push_back(commandCenter);
        }

        for (int n(0); n < factoryAddons - numFactories; ++n)
        {
            buildOrder.push_back(factory);
        }

        for (int n(0); n < starportAddons - numStarports; ++n)
        {
            buildOrder.push_back(starport);
        }
        for (int n(0); n < sciAddons - numSci; ++n)
        {
            buildOrder.push_back(scienceFacility);
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
    std::vector<ActionType> bestBuildOrder = GetNaiveBuildOrderAddWorkers(state, goal, 4);
    FrameCountType minCompletionTime = Tools::GetBuildOrderCompletionTime(state, bestBuildOrder);
    UnitCountType bestNumWorkers = GetWorkerCount(bestBuildOrder);

    for (UnitCountType numWorkers(8); numWorkers < 27; ++numWorkers)
    {
        std::vector<ActionType> buildOrder = Tools::GetNaiveBuildOrderAddWorkers(state, goal, numWorkers);
        FrameCountType completionTime = Tools::GetBuildOrderCompletionTime(state, buildOrder);
        UnitCountType workers = GetWorkerCount(buildOrder);
        
        if (completionTime <= minCompletionTime + ((workers-bestNumWorkers)*24))
        {
            minCompletionTime = completionTime;
            bestBuildOrder = buildOrder;
        }
    }

    
    FrameCountType bestCompletionTime = Tools::GetBuildOrderCompletionTime(state, bestBuildOrder);
    std::vector<ActionType> testBuildOrder;

    //std::cout << "Found a better build order that takes " << bestCompletionTime << " frames\n";
    while (true)
    {
        const static ActionType gateway = ActionTypes::GetActionType("Protoss_Gateway");
        InsertActionIntoBuildOrder(testBuildOrder, bestBuildOrder, state, gateway);

        FrameCountType completionTime = Tools::GetBuildOrderCompletionTime(state, testBuildOrder);

        if (completionTime < bestCompletionTime)
        {
            //std::cout << "Found a better build order that takes " << completionTime << " frames\n";
            bestCompletionTime = completionTime;
            bestBuildOrder = testBuildOrder;
        }
        else
        {
            break;
        }
    }


    return bestBuildOrder;
}

std::vector<ActionType> Tools::GetNaiveBuildOrderAddWorkers(const GameState & state, const DFBB_BuildOrderSearchGoal & goal, UnitCountType maxWorkers)
{
    PrerequisiteSet wanted;
    int minWorkers = 8;

    const ActionType & worker = ActionTypes::GetWorker(state.getRace());
    std::vector<size_t> buildOrderActionTypeCount(ActionTypes::GetAllActionTypes(state.getRace()).size(), 0);

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

    if (wanted.size() == 0)
    {
        return std::vector<ActionType>();
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    std::vector<ActionType> buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        const ActionType & type = requiredToBuild.getActionType(a);
        buildOrder.push_back(type);
        buildOrderActionTypeCount[type.ID()]++;
    }

    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
    int requiredWorkers = minWorkers - state.getUnitData().getNumCompleted(ActionTypes::GetWorker(state.getRace()));
    while (requiredWorkers-- > 0)
    {
        buildOrder.push_back(worker);
        buildOrderActionTypeCount[worker.ID()]++;
    }

    // Add the goal units to the end of the build order 
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
        int need = (int)goal.getGoal(actionType);
        int have = (int)state.getUnitData().getNumTotal(actionType);
        int numNeeded = need - have - buildOrderActionTypeCount[actionType.ID()]; 
            
        for (int i(0); i < numNeeded; ++i)
        {
            buildOrder.push_back(actionType);
        }
    }

    
    static const ActionType commandCenter = ActionTypes::GetActionType("Terran_Command_Center");
    static const ActionType factory = ActionTypes::GetActionType("Terran_Factory");
    static const ActionType starport = ActionTypes::GetActionType("Terran_Starport");
    static const ActionType scienceFacility = ActionTypes::GetActionType("Terran_Science_Facility");


    // Check to see if we have enough buildings for the required addons
    if (state.getRace() == Races::Terran)
    {
        int commandCenterAddons = 0;
        int factoryAddons = 0;
        int starportAddons = 0;
        int sciAddons = 0;

        int numCommandCenters = state.getUnitData().getNumTotal(commandCenter);
        int numFactories = state.getUnitData().getNumTotal(factory);
        int numStarports = state.getUnitData().getNumTotal(starport);
        int numSci = state.getUnitData().getNumTotal(scienceFacility);
        
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            const ActionType & actionType = buildOrder[a];

            if (actionType.isAddon())
            {
                if (actionType.whatBuildsActionType() == commandCenter)
                {
                    ++commandCenterAddons;
                }
                else if (actionType.whatBuildsActionType() == factory)
                {
                    ++factoryAddons;
                }
                else if (actionType.whatBuildsActionType() == starport)
                {
                    ++starportAddons;
                }
                else if (actionType.whatBuildsActionType() == scienceFacility)
                {
                    ++sciAddons;
                }
                else
                {
                    BOSS_ASSERT(false, "Addon has no builder: %s %s", actionType.getName().c_str(), actionType.whatBuildsActionType().getName().c_str());
                }
            }

            if (actionType == commandCenter)
            {
                ++numCommandCenters;
            }
            else if (actionType == factory)
            {
                ++numFactories;
            }
            else if (actionType == starport)
            {
                ++numStarports;
            }
            else if (actionType == scienceFacility)
            {
                ++numSci;
            }
        }

        // add the necessary buildings to make the addons
        for (int n(0); n < commandCenterAddons - numCommandCenters; ++n)
        {
            buildOrder.push_back(commandCenter);
        }

        for (int n(0); n < factoryAddons - numFactories; ++n)
        {
            buildOrder.push_back(factory);
        }

        for (int n(0); n < starportAddons - numStarports; ++n)
        {
            buildOrder.push_back(starport);
        }
        for (int n(0); n < sciAddons - numSci; ++n)
        {
            buildOrder.push_back(scienceFacility);
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
        if (surplusSupply < nextAction.supplyRequired() + 2)
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

        if ((numWorkers < maxWorkers) && (whenWorkerReady < whennextActionReady))
        {
            finalBuildOrder.push_back(worker);
            currentState.doAction(worker);
            continue;
        }
        else
        {
            ActionType testNextAction = buildOrder[i];
            finalBuildOrder.push_back(nextAction);

            currentState.doAction(nextAction);
            ++i;
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

void Tools::InsertActionIntoBuildOrder(std::vector<ActionType> & result, const std::vector<ActionType> & buildOrder, const GameState & initialState, const ActionType & action)
{
    int bestInsertIndex = -1;
    std::vector<ActionType> runningBuildOrder;
    GameState runningState(initialState);
    FrameCountType minCompletionTime = Tools::GetBuildOrderCompletionTime(initialState, buildOrder);

    std::vector<ActionType> testBuildOrder = buildOrder;

    for (size_t insertIndex(0); insertIndex < buildOrder.size(); ++insertIndex)
    {
        // if we can test the action here, do it
        if (runningState.isLegal(action))
        {
            // figure out the build time of build order with action inserted here
            GameState tempState(runningState);
            tempState.doAction(action);
            for (size_t a(insertIndex); a < buildOrder.size(); ++a)
            {
                tempState.doAction(buildOrder[a]);
            }

            FrameCountType completionTime = tempState.getLastActionFinishTime();

            if (completionTime < minCompletionTime)
            {
                minCompletionTime = completionTime;
                bestInsertIndex = insertIndex;
            }
        }

        BOSS_ASSERT(runningState.isLegal(buildOrder[insertIndex]), "We have made the next action illegal somehow");
        runningBuildOrder.push_back(buildOrder[insertIndex]);
        runningState.doAction(buildOrder[insertIndex]);
    }

    result.clear();
    for (size_t a(0); a<buildOrder.size(); ++a)
    {
        if (bestInsertIndex == a)
        {
            result.push_back(action);
        }

        result.push_back(buildOrder[a]);
    }
}

FrameCountType Tools::GetBuildOrderCompletionTime(const GameState & state, const std::vector<ActionType> & buildOrder, size_t buildOrderStartIndex)
{
    if (buildOrder.size() == 0)
    {
        return 0;
    }

    GameState currentState(state);

    BOSS_ASSERT(buildOrderStartIndex < buildOrder.size(), "We can't start at an index that's past the end");

    for (size_t a(buildOrderStartIndex); a < buildOrder.size(); ++a)
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

bool Tools::PerformBuildOrder(GameState & state, const std::vector<ActionType> & buildOrder)
{
    size_t i = 0;
    GameState stateCopy(state);
    try
    {
        for (i = 0; i<buildOrder.size(); ++i)
        {
            if (!state.isLegal(buildOrder[i]))
            {
                state.whyIsNotLegal(buildOrder[i]);
                return false;
            }
            else
            {
                state.doAction(buildOrder[i]);
            }
        }
    }
    catch (BOSS::Assert::BOSSException e)
    {
        std::cout << "Build order failed at: " << i << " : " << (int)buildOrder[i].ID() << ":" << buildOrder[i].getName() << " " << (int)buildOrder[i].whatBuildsActionType().ID() << ":" << buildOrder[i].whatBuildsActionType().getName() << std::endl;

        GameState errorState(stateCopy);
        for (size_t j=0; j < i; ++j)
        {
            std::cout << "Doing: " << j << " " << buildOrder[j].getName() << std::endl;
            errorState.doAction(buildOrder[j]);
        }

        for (size_t j=i; j < buildOrder.size(); ++j)
        {
            std::cout << "Doing: " << j << " " << buildOrder[j].getName() << std::endl;
            errorState.doAction(buildOrder[j]);
        }


        int a = 0;
    }

    return true;
}