#include "CombatSearch_IntegralData.h"

using namespace BOSS;

CombatSearch_IntegralData::CombatSearch_IntegralData()
    : _bestIntegralValue(0)
{
    _integralStack.push_back(IntegralData(0,0,0));
}

void CombatSearch_IntegralData::update(const GameState & state, const BuildOrder & buildOrder)
{
    double value = Eval::ArmyTotalResourceSum(state);
    double timeElapsed = state.getCurrentFrame() - _integralStack.back().timeAdded; 
    double valueToAdd = _integralStack.back().eval * timeElapsed;
    IntegralData entry(value, _integralStack.back().integral + valueToAdd, state.getCurrentFrame());
    _integralStack.push_back(entry);

    // we have found a new best if:
    // 1. the new army integral is higher than the previous best
    // 2. the new army integral is the same as the old best but the build order is 'better'
    if (    (_integralStack.back().integral >  _bestIntegralValue) 
        || ((_integralStack.back().integral == _bestIntegralValue) && Eval::BuildOrderBetter(buildOrder, _bestIntegralBuildOrder)))
    {
        _bestIntegralValue = _integralStack.back().integral;
        _bestIntegralStack = _integralStack;
        _bestIntegralBuildOrder = buildOrder;

        // print the newly found best to console
        printIntegralData(_integralStack.size()-1);
    }
}

void CombatSearch_IntegralData::pop()
{
    _integralStack.pop_back();
}

void CombatSearch_IntegralData::printIntegralData(const size_t index) const
{
    printf("%7d %10.2lf %13.2lf   ", _bestIntegralStack[index].timeAdded, _bestIntegralStack[index].eval/Constants::RESOURCE_SCALE, _bestIntegralStack[index].integral/Constants::RESOURCE_SCALE);
    std::cout << _bestIntegralBuildOrder.getNameString(2) << std::endl;   
}

void CombatSearch_IntegralData::print() const
{
    std::cout << "\nFinal CombatSearchIntegral Results\n\n";
    std::cout << "  Frame   ArmyEval  ArmyIntegral   BuildOrder\n";
    
    for (size_t i(0); i<_bestIntegralStack.size(); ++i)
    {
        printIntegralData(i);
    }
}

const BuildOrder & CombatSearch_IntegralData::getBestBuildOrder() const
{
    return _bestIntegralBuildOrder;
}