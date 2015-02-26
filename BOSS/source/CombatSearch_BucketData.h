#pragma once

#include "BuildOrder.h"
#include "Common.h"
#include "GameState.h"
#include "Eval.h"

namespace BOSS
{

class BucketData
{
public:
    double                      eval;
    BuildOrder                  buildOrder;
    GameState                   state;

    BucketData()
        : eval(0)
    {
    }
};

class CombatSearch_BucketData
{
    std::vector<BucketData>     _buckets;
    FrameCountType              _frameLimit;

    BucketData & getBucketData(const GameState & state);

public:

    CombatSearch_BucketData(const FrameCountType frameLimit, const size_t numBuckets);

    const BucketData & getBucket(const size_t index) const;
    const size_t numBuckets() const;
    const size_t getBucketIndex(const GameState & state) const;
        
    void update(const GameState & state, const BuildOrder & buildOrder);

    bool isDominated(const GameState & state);

    void print() const;
    std::string getBucketResultsString();
};

}
