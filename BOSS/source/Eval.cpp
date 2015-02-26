#include "Eval.h"

namespace BOSS
{
namespace Eval
{
    double ArmyCompletedResourceSum(const GameState & state)
    {
        ResourceCountType sum(0);
	    
        const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(state.getRace());
	    for (ActionID i(0); i<allActions.size(); ++i)
	    {
            const ActionType & a = allActions[i];
	        if (!a.isBuilding() && !a.isWorker() && !a.isSupplyProvider())
	        {
                sum += state.getUnitData().getNumCompleted(a)*a.mineralPrice();
	            sum += 2*state.getUnitData().getNumCompleted(a)*a.gasPrice();
	        }
	    }
	    
	    return sum;
    }

    double ArmyTotalResourceSum(const GameState & state)
    {
        ResourceCountType sum(0);
	    
	    const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(state.getRace());
	    for (ActionID i(0); i<allActions.size(); ++i)
	    {
            const ActionType & a = allActions[i];
	        if (!a.isBuilding() && !a.isWorker() && !a.isSupplyProvider())
	        {
                sum += state.getUnitData().getNumTotal(a)*a.mineralPrice();
	            sum += 2*state.getUnitData().getNumTotal(a)*a.gasPrice();
	        }
	    }
	    
	    return sum;
    }

    bool BuildOrderBetter(const BuildOrder & buildOrder, const BuildOrder & compareTo)
    {
        size_t numWorkers = 0;
        size_t numWorkersOther = 0;

        for (size_t a(0); a<buildOrder.size(); ++a)
        {
            if (buildOrder[a].isWorker())
            {
                numWorkers++;
            }
        }

        for (size_t a(0); a<compareTo.size(); ++a)
        {
            if (compareTo[a].isWorker())
            {
                numWorkersOther++;
            }
        }

        if (numWorkers == numWorkersOther)
        {
            return buildOrder.size() < compareTo.size();
        }
        else
        {
            return numWorkers > numWorkersOther;
        }
    }

    bool StateDominates(const GameState & state, const GameState & other)
    {
        // we can't define domination for different races
        if (state.getRace() != other.getRace())
        {
            return false;
        }

        // if we have less resources than the other state we are not dominating it
        if ((state.getMinerals() < other.getMinerals()) || (state.getGas() < other.getGas()))
        {
            return false;
        }

        // if we have less of any unit than the other state we are not dominating it
        for (size_t a(0); a < ActionTypes::GetAllActionTypes(state.getRace()).size(); ++a)
        {
            const ActionType & action = ActionTypes::GetActionType(state.getRace(), a);

            if (state.getUnitData().getNumTotal(action) < other.getUnitData().getNumTotal(action))
            {
                return false;
            }

            if (state.getUnitData().getNumCompleted(action) < other.getUnitData().getNumCompleted(action))
            {
                return false;
            }
        }

        return true;
    }
}
}

