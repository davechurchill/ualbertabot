#pragma once

#include "Common.h"
#include "GameState.h"
#include "Eval.h"
#include "BuildOrder.h"

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

class CombatSearch_IntegralData
{
    std::vector<IntegralData>       _integralStack;

    std::vector<IntegralData>       _bestIntegralStack;
    double                          _bestIntegralValue;
    BuildOrder                      _bestIntegralBuildOrder;

public:

    CombatSearch_IntegralData();

    void update(const GameState & state, const BuildOrder & buildOrder);
    void pop();

    void printIntegralData(const size_t index) const;
    void print() const;

    const BuildOrder & getBestBuildOrder() const;
};

}
