//#include "DFBB_BuildOrderSearchSaveState.h"
//
//using namespace BOSS;
//
//DFBBSearchSaveState::DFBBSearchSaveState()
//    : depth(0) 
//{
//}
//
//DFBBSearchSaveState::DFBBSearchSaveState(const std::vector<ActionType> & buildOrder, int ub)
//    : upperBound(ub)
//{
//    // set the depth equal to the size of the build order vector
//    depth = buildOrder.size();
//
//    // copy the values of the vector into the array (backwards due to buildorder vector being backwards)
//    for (size_t i(0); i<buildOrder.size(); ++i)
//    {
//        currentActions[i] = buildOrder[buildOrder.size()-1-i];
//    }
//}
//
//int DFBBSearchSaveState::getUpperBound() const
//{
//    return upperBound;
//}
//
//int DFBBSearchSaveState::operator [] (const int index) const
//{
//    return currentActions[index];
//}
//
//int DFBBSearchSaveState::getDepth() const
//{
//    return depth;
//}
//
//int DFBBSearchSaveState::getAction(const int d) const
//{
//    BOSS_ASSERT(d < depth, "Depth overflow");
//
//    return currentActions[d];
//}
//
//void DFBBSearchSaveState::print() const
//{
//    printf("Depth(%d) UpperBound(%d) Actions ",depth,upperBound);
//
//    for (int i(0); i<depth; ++i)
//    {
//        printf("%d ",currentActions[i]);
//    }
//
//    printf("\n");
//}