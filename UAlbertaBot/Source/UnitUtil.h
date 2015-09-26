#pragma once

#include <Common.h>
#include <BWAPI.h>

namespace UAlbertaBot
{
namespace UnitUtil
{      
    bool IsCombatUnit(BWAPI::Unit unit);
    bool IsValidUnit(BWAPI::Unit unit);
    bool CanAttackAir(BWAPI::Unit unit);
    bool CanAttackGround(BWAPI::Unit unit);
    bool IsGroundTarget(BWAPI::Unit unit);
    bool IsAirTarget(BWAPI::Unit unit);
    BWAPI::Unit GetTrainee(BWAPI::Unit unit);

    template <typename ValueFunction>
    BWAPI::Unit GetMinUnit(const BWAPI::Unitset & units, const ValueFunction & fn)
    {
        if (units.empty())
        {
            return nullptr;
        }
        
        auto it = units.begin();
        BWAPI::Unit minUnit = *it;
        auto min = fn(minUnit);

        for (; it != units.end(); ++it)
        {
            auto val = fn(*it);
            if (val < min)
            {
                min = val;
                minUnit = *it;
            }
        }

        return minUnit;
    }
    
    template <typename ValueFunction>
    BWAPI::Unit GetManUnit(const BWAPI::Unitset & units, const ValueFunction & fn)
    {
        if (units.empty())
        {
            return nullptr;
        }
        
        auto it = units.begin();
        BWAPI::Unit maxUnit = *it;
        auto max = fn(minUnit);

        for (; it != units.end(); ++it)
        {
            auto val = fn(*it);
            if (val > min)
            {
                max = val;
                maxUnit = *it;
            }
        }

        return maxUnit;
    }
};
}