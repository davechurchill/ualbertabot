#include "DFBB_BuildOrderSearchResults.h"

using namespace BOSS;

DFBB_BuildOrderSearchResults::DFBB_BuildOrderSearchResults()
    : solved(false)
    , timedOut(false)
    , solutionFound(false)
    , upperBound(0)
    , nodesExpanded(0)
    , timeElapsed(0)
{
}

void DFBB_BuildOrderSearchResults::printResults(bool pbo) const
{
    printf("%12d%14llu%12.2lf       ",upperBound,nodesExpanded,timeElapsed);

    if (pbo)
    {
        printBuildOrder();
    }

    printf("\n");
}

void DFBB_BuildOrderSearchResults::printBuildOrder() const
{
    for (size_t i(0); i<buildOrder.size(); ++i)
    {
        printf("%d ",buildOrder[i].ID());
    }
}