#include "Tools.h"
#include "BuildOrderSearchGoal.h"
#include "NaiveBuildOrderSearch.h"

using namespace BOSS;

#include "JSONTools.h"

//BuildOrder Tools::GetNaiveBuildOrder(const GameState & state, const BuildOrderSearchGoal & goal)
//{
//    PrerequisiteSet wanted;
//    int minWorkers = 12;
//
//    const ActionType & worker = ActionTypes::GetWorker(state.getRace());
//    std::vector<size_t> buildOrderActionTypeCount(ActionTypes::GetAllActionTypes(state.getRace()).size(), 0);
//
//    // add everything from the goal to the needed set
//    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
//    {
//        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
//        UnitCountType numCompleted = state.getUnitData().getNumTotal(actionType);
//            
//        if (goal.getGoal(actionType) > numCompleted)
//        {
//            wanted.addUnique(actionType);
//        }
//    }
//
//    if (wanted.size() == 0)
//    {
//        return BuildOrder();
//    }
//
//    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
//    PrerequisiteSet requiredToBuild;
//    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);
//
//    // Add the required units to a preliminary build order
//    BuildOrder buildOrder;
//    for (size_t a(0); a < requiredToBuild.size(); ++a)
//    {
//        const ActionType & type = requiredToBuild.getActionType(a);
//        buildOrder.add(type);
//        buildOrderActionTypeCount[type.ID()]++;
//    }
//
//    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
//    int requiredWorkers = minWorkers - state.getUnitData().getNumCompleted(ActionTypes::GetWorker(state.getRace()));
//    while (requiredWorkers-- > 0)
//    {
//        buildOrder.add(worker);
//        buildOrderActionTypeCount[worker.ID()]++;
//    }
//
//    // Add the goal units to the end of the build order 
//    for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
//    {
//        const ActionType & actionType = ActionTypes::GetActionType(state.getRace(), a);
//        int need = (int)goal.getGoal(actionType);
//        int have = (int)state.getUnitData().getNumTotal(actionType);
//        int numNeeded = need - have - buildOrderActionTypeCount[actionType.ID()];
//         
//        for (int i(0); i < numNeeded; ++i)
//        {
//            buildOrder.add(actionType);
//            buildOrderActionTypeCount[actionType.ID()]++;
//        }
//    }
//
//    // if we are zerg, make sure we have enough morphers for morphed units
//    if (state.getRace() == Races::Zerg)
//    {
//        // do this whole thing twice so that Hive->Lair->Hatchery is satisfied
//        for (size_t t=0; t<2; ++t)
//        {
//            std::vector<size_t> neededMorphers(ActionTypes::GetAllActionTypes(state.getRace()).size(), 0);
//            for (size_t i(0); i < buildOrderActionTypeCount.size(); ++i)
//            {
//                const ActionType & type = ActionTypes::GetActionType(Races::Zerg, i);
//
//                if (type.isMorphed())
//                {
//                    const ActionType & morpher = type.whatBuildsActionType();
//
//                    int willMorph = buildOrderActionTypeCount[i];
//                    int haveMorpher = state.getUnitData().getNumTotal(morpher);
//                    int boMoprher = buildOrderActionTypeCount[morpher.ID()];
//
//                    int need = willMorph - haveMorpher - boMoprher;
//
//                    if (need > 0)
//                    {
//                        neededMorphers[morpher.ID()] += need;
//                    }
//                }
//            }
//
//            // add the morphers to the build order
//            for (size_t i(0); i<neededMorphers.size(); ++i)
//            {
//                for (size_t n(0); n < neededMorphers[i]; ++n)
//                {
//                    buildOrder.add(ActionTypes::GetActionType(Races::Zerg, i));
//                    buildOrderActionTypeCount[i]++;
//                }
//            }
//        }
//
//        // special case: hydra/lurker both in goal, need to add hydras, same with creep/sunken and muta/guardian
//        // ignore other spire / hatchery since they recursively serve all purposes
//        static const ActionType & Hydralisk     = ActionTypes::GetActionType("Zerg_Hydralisk");
//        static const ActionType & Lurker        = ActionTypes::GetActionType("Zerg_Lurker");
//        static const ActionType & Creep         = ActionTypes::GetActionType("Zerg_Creep_Colony");
//        static const ActionType & Sunken        = ActionTypes::GetActionType("Zerg_Sunken_Colony");
//        static const ActionType & Spore         = ActionTypes::GetActionType("Zerg_Spore_Colony");
//        static const ActionType & Mutalisk      = ActionTypes::GetActionType("Zerg_Mutalisk");
//        static const ActionType & Guardian      = ActionTypes::GetActionType("Zerg_Guardian");
//        static const ActionType & Devourer      = ActionTypes::GetActionType("Zerg_Devourer");
//
//        if (goal.getGoal(Hydralisk) > 0)
//        {
//            int currentHydras = state.getUnitData().getNumTotal(Hydralisk) + buildOrderActionTypeCount[Hydralisk.ID()] - buildOrderActionTypeCount[Lurker.ID()];
//            int additionalHydras = goal.getGoal(Hydralisk) - currentHydras;
//
//            for (int i=0; i<additionalHydras; ++i)
//            {
//                buildOrder.add(Hydralisk);
//                buildOrderActionTypeCount[Hydralisk.ID()]++;
//            }
//        }
//
//        if (goal.getGoal(Guardian) > 0 && goal.getGoal(Devourer) > 0)
//        {
//            int currentMutas = state.getUnitData().getNumTotal(Mutalisk) + buildOrderActionTypeCount[Mutalisk.ID()];
//            int additionalMutas = buildOrderActionTypeCount[Guardian.ID()] + buildOrderActionTypeCount[Devourer.ID()] - currentMutas;
//
//            for (int i=0; i<additionalMutas; ++i)
//            {
//                buildOrder.add(Mutalisk);
//                buildOrderActionTypeCount[Mutalisk.ID()]++;
//            }
//        }
//
//        if (goal.getGoal(Mutalisk) > 0)
//        {
//            int currentMutas = state.getUnitData().getNumTotal(Mutalisk) + buildOrderActionTypeCount[Mutalisk.ID()] - buildOrderActionTypeCount[Guardian.ID()] - buildOrderActionTypeCount[Devourer.ID()];
//            int additionalMutas = goal.getGoal(Mutalisk) - currentMutas;
//
//            for (int i=0; i<additionalMutas; ++i)
//            {
//                buildOrder.add(Mutalisk);
//                buildOrderActionTypeCount[Mutalisk.ID()]++;
//            }
//        }
//
//        if (goal.getGoal(Sunken) > 0 && goal.getGoal(Spore) > 0)
//        {
//            int currentCreep = state.getUnitData().getNumTotal(Creep) + buildOrderActionTypeCount[Creep.ID()];
//            int additionalCreep = buildOrderActionTypeCount[Spore.ID()] + buildOrderActionTypeCount[Sunken.ID()] - currentCreep;
//
//            for (int i=0; i<additionalCreep; ++i)
//            {
//                buildOrder.add(Creep);
//                buildOrderActionTypeCount[Creep.ID()]++;
//            }
//        }
//
//        if (goal.getGoal(Creep) > 0)
//        {
//            int currentCreep = state.getUnitData().getNumTotal(Creep) + buildOrderActionTypeCount[Creep.ID()] - buildOrderActionTypeCount[Spore.ID()] - buildOrderActionTypeCount[Sunken.ID()];
//            int additionalCreep = goal.getGoal(Creep) - currentCreep;
//
//            for (int i=0; i<additionalCreep; ++i)
//            {
//                buildOrder.add(Creep);
//                buildOrderActionTypeCount[Creep.ID()]++;
//            }
//        }
//    }
//
//    // figure out how many workers are needed for the build order to be legal      
//    size_t workersNeeded = 2 + goal.getGoal(worker);
//
//    // we need enough workers to fill all the refineries that will be built
//    workersNeeded += 3*state.getUnitData().getNumTotal(ActionTypes::GetRefinery(state.getRace()));
//    workersNeeded += 3*buildOrderActionTypeCount[ActionTypes::GetRefinery(state.getRace()).ID()];
//
//    // special case for zerg: buildings consume drones
//    if (state.getRace() == Races::Zerg)
//    {
//        workersNeeded += 3*buildOrderActionTypeCount[ActionTypes::GetRefinery(state.getRace()).ID()];
//
//        for (size_t i(0); i < buildOrderActionTypeCount.size(); ++i)
//        {
//            const ActionType & type = ActionTypes::GetActionType(Races::Zerg, i);
//
//            if (type.whatBuildsActionType().isWorker() && !type.isMorphed())
//            {
//                workersNeeded += buildOrderActionTypeCount[i];
//            }
//        }
//    }
//
//    int workersToAdd = workersNeeded - state.getUnitData().getNumTotal(worker) - buildOrderActionTypeCount[worker.ID()];
//    for (int i(0); i < workersToAdd; ++i)
//    {
//        buildOrder.add(worker);
//        buildOrderActionTypeCount[worker.ID()]++;
//    }
//
//
//    // Check to see if we have enough buildings for the required addons
//    if (state.getRace() == Races::Terran)
//    {
//        int commandCenterAddons = 0;
//        int factoryAddons = 0;
//        int starportAddons = 0;
//        int sciAddons = 0;
//
//        static const ActionType commandCenter = ActionTypes::GetActionType("Terran_Command_Center");
//        static const ActionType factory = ActionTypes::GetActionType("Terran_Factory");
//        static const ActionType starport = ActionTypes::GetActionType("Terran_Starport");
//        static const ActionType scienceFacility = ActionTypes::GetActionType("Terran_Science_Facility");
//
//        int numCommandCenters = state.getUnitData().getNumTotal(commandCenter);
//        int numFactories = state.getUnitData().getNumTotal(factory);
//        int numStarports = state.getUnitData().getNumTotal(starport);
//        int numSci = state.getUnitData().getNumTotal(scienceFacility);
//        
//        for (size_t a(0); a < buildOrder.size(); ++a)
//        {
//            const ActionType & actionType = buildOrder[a];
//
//            if (actionType.isAddon())
//            {
//                if (actionType.whatBuildsActionType() == commandCenter)
//                {
//                    ++commandCenterAddons;
//                }
//                else if (actionType.whatBuildsActionType() == factory)
//                {
//                    ++factoryAddons;
//                }
//                else if (actionType.whatBuildsActionType() == starport)
//                {
//                    ++starportAddons;
//                }
//                else if (actionType.whatBuildsActionType() == scienceFacility)
//                {
//                    ++sciAddons;
//                }
//                else
//                {
//                    BOSS_ASSERT(false, "Inconceivable: %s", actionType.getName().c_str());
//                }
//            }
//
//            if (actionType == commandCenter)
//            {
//                ++numCommandCenters;
//            }
//            else if (actionType == factory)
//            {
//                ++numFactories;
//            }
//            else if (actionType == starport)
//            {
//                ++numStarports;
//            }
//            else if (actionType == scienceFacility)
//            {
//                ++numSci;
//            }
//        }
//
//        // add the necessary buildings to make the addons
//        for (int n(0); n < commandCenterAddons - numCommandCenters; ++n)
//        {
//            buildOrder.add(commandCenter);
//        }
//
//        for (int n(0); n < factoryAddons - numFactories; ++n)
//        {
//            buildOrder.add(factory);
//        }
//
//        for (int n(0); n < starportAddons - numStarports; ++n)
//        {
//            buildOrder.add(starport);
//        }
//        for (int n(0); n < sciAddons - numSci; ++n)
//        {
//            buildOrder.add(scienceFacility);
//        }
//    }
//
//    // Bubble sort the build order so that prerequites always come before what requires them
//    for (size_t i(0); i < buildOrder.size()-1; ++i)
//    {
//        for (size_t j(i+1); j < buildOrder.size(); ++j)
//        {
//            const PrerequisiteSet & recursivePre = buildOrder[i].getRecursivePrerequisites();
//
//            if (recursivePre.contains(buildOrder[j]))
//            {
//                std::swap(buildOrder[i], buildOrder[j]);
//            }
//        }
//    }
//
//    // Insert supply buildings so that build order is legal w.r.t. supply counts
//    int maxSupply = state.getUnitData().getMaxSupply() + state.getUnitData().getSupplyInProgress();
//    int currentSupply = state.getUnitData().getCurrentSupply();
//
//    GameState currentState(state);
//    const ActionType & supplyProvider = ActionTypes::GetSupplyProvider(state.getRace());
//
//    BuildOrder finalBuildOrder;
//    for (size_t a(0); a < buildOrder.size(); ++a)
//    {
//        const ActionType & nextAction = buildOrder[a];
//        UnitCountType maxSupply = currentState.getUnitData().getMaxSupply();
//        UnitCountType currentSupply = currentState.getUnitData().getCurrentSupply();
//        UnitCountType supplyInProgress = currentState.getUnitData().getSupplyInProgress();
//
//        // insert a supply provider if we need one
//        if (!nextAction.isMorphed() && (nextAction.supplyRequired() > (maxSupply+supplyInProgress-currentSupply)))
//        {
//            if (!currentState.isLegal(supplyProvider))
//            {
//                currentState.whyIsNotLegal(supplyProvider);
//
//                for (size_t i(0); i<finalBuildOrder.size(); ++i)
//                {
//                    std::cout << i << " " << finalBuildOrder[i].getName() << std::endl;
//                }
//
//                std::cout << finalBuildOrder.getJSONString() << std::endl;
//            }
//
//            BOSS_ASSERT(currentState.isLegal(supplyProvider), "Should be able to build more supply here");
//            finalBuildOrder.add(supplyProvider);
//            currentState.doAction(supplyProvider);
//        }
//
//        if (!currentState.isLegal(nextAction))
//        {
//            currentState.isLegal(nextAction);
//            currentState.whyIsNotLegal(nextAction);
//        }
//
//        BOSS_ASSERT(currentState.isLegal(nextAction), "Should be able to build the next action now");
//        finalBuildOrder.add(nextAction);
//        currentState.doAction(nextAction);
//    }
//
//    return finalBuildOrder;
//}

BuildOrder Tools::GetOptimizedNaiveBuildOrder(const GameState & state, const BuildOrderSearchGoal & goal)
{
    BuildOrder bestBuildOrder = GetNaiveBuildOrderAddWorkers(state, goal, 4);
    FrameCountType minCompletionTime = bestBuildOrder.getCompletionTime(state);
    UnitCountType bestNumWorkers = bestBuildOrder.getTypeCount(ActionTypes::GetWorker(state.getRace()));

    for (UnitCountType numWorkers(8); numWorkers < 27; ++numWorkers)
    {
        BuildOrder buildOrder = Tools::GetNaiveBuildOrderAddWorkers(state, goal, numWorkers);
        FrameCountType completionTime = buildOrder.getCompletionTime(state);
        UnitCountType workers = buildOrder.getTypeCount(ActionTypes::GetWorker(state.getRace()));
        
        if (completionTime <= minCompletionTime + ((workers-bestNumWorkers)*24))
        {
            minCompletionTime = completionTime;
            bestBuildOrder = buildOrder;
        }
    }

    
    FrameCountType bestCompletionTime = bestBuildOrder.getCompletionTime(state);
    BuildOrder testBuildOrder;

    //std::cout << "Found a better build order that takes " << bestCompletionTime << " frames\n";
    while (true)
    {
        const static ActionType gateway = ActionTypes::GetActionType("Protoss_Gateway");
        InsertActionIntoBuildOrder(testBuildOrder, bestBuildOrder, state, gateway);

        FrameCountType completionTime = testBuildOrder.getCompletionTime(state);

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

BuildOrder Tools::GetNaiveBuildOrderAddWorkers(const GameState & state, const BuildOrderSearchGoal & goal, UnitCountType maxWorkers)
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
        return BuildOrder();
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    CalculatePrerequisitesRequiredToBuild(state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    BuildOrder buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        const ActionType & type = requiredToBuild.getActionType(a);
        buildOrder.add(type);
        buildOrderActionTypeCount[type.ID()]++;
    }

    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
    int requiredWorkers = minWorkers - state.getUnitData().getNumCompleted(ActionTypes::GetWorker(state.getRace()));
    while (requiredWorkers-- > 0)
    {
        buildOrder.add(worker);
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
            buildOrder.add(actionType);
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
            buildOrder.add(commandCenter);
        }

        for (int n(0); n < factoryAddons - numFactories; ++n)
        {
            buildOrder.add(factory);
        }

        for (int n(0); n < starportAddons - numStarports; ++n)
        {
            buildOrder.add(starport);
        }
        for (int n(0); n < sciAddons - numSci; ++n)
        {
            buildOrder.add(scienceFacility);
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
    BuildOrder finalBuildOrder;
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
            finalBuildOrder.add(worker);
            currentState.doAction(worker);
            continue;
        }

        // insert a supply provider if we are behind
        int surplusSupply = maxSupply - currentSupply;
        if (surplusSupply < nextAction.supplyRequired() + 2)
        {
            BOSS_ASSERT(currentState.isLegal(supplyProvider), "supplyProvider should be legal");
            finalBuildOrder.add(supplyProvider);
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
            finalBuildOrder.add(worker);
            currentState.doAction(worker);
            continue;
        }
        else
        {
            ActionType testNextAction = buildOrder[i];
            finalBuildOrder.add(nextAction);

            currentState.doAction(nextAction);
            ++i;
        }
    }

    return finalBuildOrder;
}

void Tools::InsertActionIntoBuildOrder(BuildOrder & result, const BuildOrder & buildOrder, const GameState & initialState, const ActionType & action)
{
    int bestInsertIndex = -1;
    BuildOrder runningBuildOrder;
    GameState runningState(initialState);
    FrameCountType minCompletionTime = buildOrder.getCompletionTime(initialState);

    BuildOrder testBuildOrder = buildOrder;

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
        runningBuildOrder.add(buildOrder[insertIndex]);
        runningState.doAction(buildOrder[insertIndex]);
    }

    result.clear();
    for (size_t a(0); a<buildOrder.size(); ++a)
    {
        if (bestInsertIndex == a)
        {
            result.add(action);
        }

        result.add(buildOrder[a]);
    }
}

FrameCountType Tools::GetUpperBound(const GameState & state, const BuildOrderSearchGoal & goal)
{
    NaiveBuildOrderSearch naiveSearch(state, goal);
    const BuildOrder & naiveBuildOrder = naiveSearch.solve();
    FrameCountType upperBound = naiveBuildOrder.getCompletionTime(state);

    return upperBound;
}

FrameCountType Tools::GetLowerBound(const GameState & state, const BuildOrderSearchGoal & goal)
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

