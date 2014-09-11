#include "CombatSearchIntegral.h"

using namespace BOSS;

CombatSearchIntegral::CombatSearchIntegral()
    : _bestIntegralValue(0)
{
    _integralStack.push_back(IntegralData(0,0,0));
}

void CombatSearchIntegral::update(const GameState & state, const std::vector<ActionType> & buildOrder)
{
    double value = Eval::ArmyTotalResourceSum(state);
    double timeElapsed = state.getCurrentFrame() - _integralStack.back().timeAdded; 
    double valueToAdd = _integralStack.back().eval * timeElapsed;
    IntegralData entry(value, _integralStack.back().integral + valueToAdd, state.getCurrentFrame());
    _integralStack.push_back(entry);

    if (    (_integralStack.back().integral >  _bestIntegralValue) 
        || ((_integralStack.back().integral == _bestIntegralValue) && Eval::BuildOrderBetter(buildOrder, _bestIntegralBuildOrder)))
    {
        _bestIntegralValue = _integralStack.back().integral;
        _bestIntegralStack = _integralStack;
        _bestIntegralBuildOrder = buildOrder;

        printBest();
    }
}

void CombatSearchIntegral::pop()
{
    _integralStack.pop_back();
}

void CombatSearchIntegral::printBest() const
{
    std::cout << (int)_bestIntegralStack.back().timeAdded << "\t" << (int)_bestIntegralStack.back().integral << "\t";

    for (size_t a(0); a < _bestIntegralBuildOrder.size(); ++a)
    {
        std::cout << (int)_bestIntegralBuildOrder[a].ID() << " ";
    }   std::cout << std::endl;
}

void CombatSearchIntegral::print() const
{
    std::cout << "\n";
    for (size_t i(0); i<_bestIntegralStack.size(); ++i)
    {
        std::cout << (int)_bestIntegralStack[i].timeAdded << "\t" << _bestIntegralStack[i].eval << "\t" << _bestIntegralStack[i].integral << "\t";

        for (size_t a(0); a < i; ++a)
        {
            std::cout << (int)_bestIntegralBuildOrder[a].ID() << " ";
        }   std::cout << std::endl;
    }
}