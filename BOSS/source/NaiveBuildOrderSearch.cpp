#include "NaiveBuildOrderSearch.h"

using namespace BOSS;

NaiveBuildOrderSearch::NaiveBuildOrderSearch(const GameState & state, const BuildOrderSearchGoal & goal)
    : _state(state)
    , _goal(goal)
    , _naiveSolved(false)
{

}

const BuildOrder & NaiveBuildOrderSearch::solve()
{
    if (_naiveSolved)
    {
        return _buildOrder;
    }

    PrerequisiteSet wanted;
    int minWorkers = 12;

    const ActionType & worker = ActionTypes::GetWorker(_state.getRace());

    // add everything from the goal to the needed set
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(_state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(_state.getRace(), a);
        UnitCountType numCompleted = _state.getUnitData().getNumTotal(actionType);
            
        if (_goal.getGoal(actionType) > numCompleted)
        {
            wanted.addUnique(actionType);
        }
    }

    if (wanted.size() == 0)
    {
        return _buildOrder;
    }

    // Calculate which prerequisite units we need to build to achieve the units we want from the goal
    PrerequisiteSet requiredToBuild;
    Tools::CalculatePrerequisitesRequiredToBuild(_state, wanted, requiredToBuild);

    // Add the required units to a preliminary build order
    BuildOrder buildOrder;
    for (size_t a(0); a < requiredToBuild.size(); ++a)
    {
        const ActionType & type = requiredToBuild.getActionType(a);
        buildOrder.add(type);
    }

    // Add some workers to the build order if we don't have many, this usually gives a lower upper bound
    int requiredWorkers = minWorkers - _state.getUnitData().getNumCompleted(ActionTypes::GetWorker(_state.getRace()));
    buildOrder.add(worker, requiredWorkers);

    // Add the goal units to the end of the build order 
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(_state.getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(_state.getRace(), a);
        int need = (int)_goal.getGoal(actionType);
        int have = (int)_state.getUnitData().getNumTotal(actionType);
        int numNeeded = need - have - buildOrder.getTypeCount(actionType);
         
        buildOrder.add(actionType, numNeeded);
    }

    // if we are zerg, make sure we have enough morphers for morphed units
    if (_state.getRace() == Races::Zerg)
    {
        // do this whole thing twice so that Hive->Lair->Hatchery is satisfied
        for (size_t t=0; t<2; ++t)
        {
            std::vector<size_t> neededMorphers(ActionTypes::GetAllActionTypes(_state.getRace()).size(), 0);
            for (size_t i(0); i < ActionTypes::GetAllActionTypes(_state.getRace()).size(); ++i)
            {
                const ActionType & type = ActionTypes::GetActionType(Races::Zerg, i);

                if (type.isMorphed())
                {
                    const ActionType & morpher = type.whatBuildsActionType();

                    int willMorph = buildOrder.getTypeCount(type);
                    int haveMorpher = _state.getUnitData().getNumTotal(morpher);
                    int boMoprher = buildOrder.getTypeCount(morpher);

                    int need = willMorph - haveMorpher - boMoprher;

                    if (need > 0)
                    {
                        neededMorphers[morpher.ID()] += need;
                    }
                }
            }
            
            // add the morphers to the build order
            for (size_t i(0); i<neededMorphers.size(); ++i)
            {
                buildOrder.add(ActionTypes::GetActionType(Races::Zerg, i), neededMorphers[i]);
            }
        }

        // special case: hydra/lurker both in goal, need to add hydras, same with creep/sunken and muta/guardian
        // ignore other spire / hatchery since they recursively serve all purposes
        static const ActionType & Hydralisk     = ActionTypes::GetActionType("Zerg_Hydralisk");
        static const ActionType & Lurker        = ActionTypes::GetActionType("Zerg_Lurker");
        static const ActionType & Creep         = ActionTypes::GetActionType("Zerg_Creep_Colony");
        static const ActionType & Sunken        = ActionTypes::GetActionType("Zerg_Sunken_Colony");
        static const ActionType & Spore         = ActionTypes::GetActionType("Zerg_Spore_Colony");
        static const ActionType & Mutalisk      = ActionTypes::GetActionType("Zerg_Mutalisk");
        static const ActionType & Guardian      = ActionTypes::GetActionType("Zerg_Guardian");
        static const ActionType & Devourer      = ActionTypes::GetActionType("Zerg_Devourer");

        if (_goal.getGoal(Hydralisk) > 0)
        {
            int currentHydras = _state.getUnitData().getNumTotal(Hydralisk) + buildOrder.getTypeCount(Hydralisk) - buildOrder.getTypeCount(Lurker);
            int additionalHydras = _goal.getGoal(Hydralisk) - currentHydras;
            buildOrder.add(Hydralisk, additionalHydras);
        }

        if (_goal.getGoal(Guardian) > 0 && _goal.getGoal(Devourer) > 0)
        {
            int currentMutas = _state.getUnitData().getNumTotal(Mutalisk) + buildOrder.getTypeCount(Mutalisk);
            int additionalMutas = buildOrder.getTypeCount(Guardian) + buildOrder.getTypeCount(Devourer) - currentMutas;
            buildOrder.add(Mutalisk, additionalMutas);
        }

        if (_goal.getGoal(Mutalisk) > 0)
        {
            int currentMutas = _state.getUnitData().getNumTotal(Mutalisk) + buildOrder.getTypeCount(Mutalisk) - buildOrder.getTypeCount(Guardian) - buildOrder.getTypeCount(Devourer);
            int additionalMutas = _goal.getGoal(Mutalisk) - currentMutas;
            buildOrder.add(Mutalisk, additionalMutas);
        }

        if (_goal.getGoal(Sunken) > 0 && _goal.getGoal(Spore) > 0)
        {
            int currentCreep = _state.getUnitData().getNumTotal(Creep) + buildOrder.getTypeCount(Creep);
            int additionalCreep = buildOrder.getTypeCount(Spore) + buildOrder.getTypeCount(Sunken) - currentCreep;
            buildOrder.add(Creep, additionalCreep);
        }

        if (_goal.getGoal(Creep) > 0)
        {
            int currentCreep = _state.getUnitData().getNumTotal(Creep) + buildOrder.getTypeCount(Creep) - buildOrder.getTypeCount(Spore) - buildOrder.getTypeCount(Sunken);
            int additionalCreep = _goal.getGoal(Creep) - currentCreep;
            buildOrder.add(Creep, additionalCreep);
        }
    }

    // figure out how many workers are needed for the build order to be legal      
    size_t workersNeeded = 2 + _goal.getGoal(worker);

    // we need enough workers to fill all the refineries that will be built
    workersNeeded += 3*_state.getUnitData().getNumTotal(ActionTypes::GetRefinery(_state.getRace()));
    workersNeeded += 3*buildOrder.getTypeCount(ActionTypes::GetRefinery(_state.getRace()));

    // special case for zerg: buildings consume drones
    if (_state.getRace() == Races::Zerg)
    {
        workersNeeded += 3*buildOrder.getTypeCount(ActionTypes::GetRefinery(_state.getRace()));

        for (size_t i(0); i < ActionTypes::GetAllActionTypes(_state.getRace()).size(); ++i)
        {
            const ActionType & type = ActionTypes::GetActionType(Races::Zerg, i);

            if (type.whatBuildsActionType().isWorker() && !type.isMorphed())
            {
                workersNeeded += buildOrder.getTypeCount(type);
            }
        }
    }

    int workersToAdd = workersNeeded - _state.getUnitData().getNumTotal(worker) - buildOrder.getTypeCount(worker);
    buildOrder.add(worker, workersNeeded);

    // Check to see if we have enough buildings for the required addons
    if (_state.getRace() == Races::Terran)
    {
        // Terran buildings that can make addons
        static const ActionType CommandCenter   = ActionTypes::GetActionType("Terran_Command_Center");
        static const ActionType Factory         = ActionTypes::GetActionType("Terran_Factory");
        static const ActionType Starport        = ActionTypes::GetActionType("Terran_Starport");
        static const ActionType ScienceFacility = ActionTypes::GetActionType("Terran_Science_Facility");

        // Terran building addons
        static const ActionType ComsatStation   = ActionTypes::GetActionType("Terran_Comsat_Station");
        static const ActionType NuclearSilo     = ActionTypes::GetActionType("Terran_Nuclear_Silo");
        static const ActionType MachineShop     = ActionTypes::GetActionType("Terran_Machine_Shop");
        static const ActionType ControlTower    = ActionTypes::GetActionType("Terran_Control_Tower");
        static const ActionType PhysicsLab      = ActionTypes::GetActionType("Terran_Physics_Lab");
        static const ActionType CovertOps       = ActionTypes::GetActionType("Terran_Covert_Ops");

        int numCommandCenters   = _state.getUnitData().getNumTotal(CommandCenter)   + buildOrder.getTypeCount(CommandCenter);
        int numFactories        = _state.getUnitData().getNumTotal(Factory)         + buildOrder.getTypeCount(Factory);
        int numStarports        = _state.getUnitData().getNumTotal(Starport)        + buildOrder.getTypeCount(Starport);
        int numSci              = _state.getUnitData().getNumTotal(ScienceFacility) + buildOrder.getTypeCount(ScienceFacility);

        int commandCenterAddons = buildOrder.getTypeCount(ComsatStation) + buildOrder.getTypeCount(NuclearSilo);
        int factoryAddons       = buildOrder.getTypeCount(MachineShop);
        int starportAddons      = buildOrder.getTypeCount(ControlTower);
        int sciAddons           = buildOrder.getTypeCount(PhysicsLab) + buildOrder.getTypeCount(CovertOps);
        
        // add the necessary buildings to make the addons
        buildOrder.add(CommandCenter, commandCenterAddons - numCommandCenters);
        buildOrder.add(Factory, factoryAddons - numFactories);
        buildOrder.add(Starport, starportAddons - numStarports);
        buildOrder.add(ScienceFacility, sciAddons - numSci);
    }

    // Bubble sort the build order so that prerequites always come before what requires them
    buildOrder.sortByPrerequisites();

    // Insert supply buildings so that build order is legal w.r.t. supply counts
    int maxSupply = _state.getUnitData().getMaxSupply() + _state.getUnitData().getSupplyInProgress();
    int currentSupply = _state.getUnitData().getCurrentSupply();

    const ActionType & supplyProvider = ActionTypes::GetSupplyProvider(_state.getRace());

    BuildOrder finalBuildOrder;
    for (size_t a(0); a < buildOrder.size(); ++a)
    {
        const ActionType & nextAction = buildOrder[a];
        UnitCountType maxSupply = _state.getUnitData().getMaxSupply();
        UnitCountType currentSupply = _state.getUnitData().getCurrentSupply();
        UnitCountType supplyInProgress = _state.getUnitData().getSupplyInProgress();

		// insert 1 or more supply providers if needed
		while (!nextAction.isMorphed() && !nextAction.isSupplyProvider() && (nextAction.supplyRequired() > (maxSupply + supplyInProgress - currentSupply)))
		{
			BOSS_ASSERT(_state.isLegal(supplyProvider), "Should be able to build more supply here. Max: %d", maxSupply);
			finalBuildOrder.add(supplyProvider);
			_state.doAction(supplyProvider);

			maxSupply = _state.getUnitData().getMaxSupply();
			currentSupply = _state.getUnitData().getCurrentSupply();
			supplyInProgress = _state.getUnitData().getSupplyInProgress();
		}

		BOSS_ASSERT(_state.isLegal(nextAction), "Should be able to build the next action now");
		finalBuildOrder.add(nextAction);
		_state.doAction(nextAction);
	}

    _buildOrder = finalBuildOrder;
    _naiveSolved = true;
    return _buildOrder;
}

