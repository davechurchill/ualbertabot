#pragma once

#include "Common.h"
#include "GameState.h"
#include "Eval.h"

namespace BOSS
{

class BucketData
{
public:
    double                      eval;
    std::vector<ActionType>     buildOrder;
    GameState                   state;

    BucketData()
        : eval(0)
    {
    }
};

class CombatSearchBucket
{
    std::vector<BucketData>     _buckets;
    FrameCountType              _frameLimit;

    
    BucketData & getBucketData(const GameState & state);

public:

    CombatSearchBucket(const FrameCountType frameLimit, const size_t numBuckets);

    const size_t numBuckets() const;
    const size_t getBucketIndex(const GameState & state) const;

    
    void update(const GameState & state, const std::vector<ActionType> & buildOrder);

    bool isDominated(const GameState & state);

    void print() const;
};

}
