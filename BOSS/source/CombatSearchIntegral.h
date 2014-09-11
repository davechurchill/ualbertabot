#pragma once

#include "Common.h"
#include "GameState.h"
#include "Eval.h"

namespace BOSS
{
    
class IntegralData
{
public:
    double              eval;
    double              integral;
    FrameCountType      timeAdded;

    IntegralData(double e, double i, FrameCountType t)
        : eval(e)
        , integral(i)
        , timeAdded(t)
    {
    
    }
    
    IntegralData()
        : eval(0)
        , integral(0)
        , timeAdded(0)
    {
    
    }

};

class CombatSearchIntegral
{
    std::vector<IntegralData>       _integralStack;

    std::vector<IntegralData>       _bestIntegralStack;
    double                          _bestIntegralValue;
    std::vector<ActionType>         _bestIntegralBuildOrder;

public:

    CombatSearchIntegral();

    void update(const GameState & state, const std::vector<ActionType> & buildOrder);
    void pop();

    void printBest() const;
    void print() const;
};

}
