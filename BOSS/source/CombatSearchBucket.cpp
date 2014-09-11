#include "CombatSearchBucket.h"

using namespace BOSS;

CombatSearchBucket::CombatSearchBucket(const FrameCountType frameLimit, const size_t numBuckets)
        : _buckets(numBuckets, BucketData())
        , _frameLimit(frameLimit)
{
    
}

const size_t CombatSearchBucket::numBuckets() const
{
    return _buckets.size();
}

const size_t CombatSearchBucket::getBucketIndex(const GameState & state) const
{
    return (size_t)(((double)state.getCurrentFrame() / (double)_frameLimit) * _buckets.size());
}

void CombatSearchBucket::update(const GameState & state, const std::vector<ActionType> & buildOrder)
{
    if (state.getCurrentFrame() >= _frameLimit)
    {
        return;
    }

    BOSS_ASSERT(state.getCurrentFrame() <= _frameLimit, "State's frame exceeds bucket frame limit: (%d %d)", (int)state.getCurrentFrame(), (int)_frameLimit);

    size_t bucketIndex = getBucketIndex(state);

    double eval = Eval::ArmyTotalResourceSum(state);

    BucketData & bucket = _buckets[bucketIndex];
    if ((eval > bucket.eval) || ((eval == bucket.eval) && Eval::BuildOrderBetter(buildOrder, bucket.buildOrder)))
    {
        for (size_t b=bucketIndex; b < _buckets.size(); ++b)
        {
            if (_buckets[b].eval >= eval)
            {
                break;
            }

            _buckets[b].eval = eval;
            _buckets[b].buildOrder = buildOrder;
            _buckets[b].state = state;
        }
    }
}

bool CombatSearchBucket::isDominated(const GameState & state)
{
    return Eval::StateDominates(getBucketData(state).state, state);
}

BucketData & CombatSearchBucket::getBucketData(const GameState & state)
{
    BOSS_ASSERT(getBucketIndex(state) < _buckets.size(), "State goes over bucket limit");

    return _buckets[getBucketIndex(state)];
}

void CombatSearchBucket::print() const
{
    std::cout << "\n\n";
    double maxEval = 0;

    for (size_t b(0); b<_buckets.size(); ++b)
    {
        if (_buckets[b].eval > maxEval)
        {
            maxEval = _buckets[b].eval;

            double frame = ((double)b / _buckets.size()) * _frameLimit;
            double sec   = frame / 24;
            std::cout << (int)frame << "\t" << (int)sec << "\t" << _buckets[b].eval << "\t";

            for (size_t a(0); a < _buckets[b].buildOrder.size(); ++a)
            {
                std::cout << (int)_buckets[b].buildOrder[a].ID() << " ";
            }   std::cout << std::endl;
        }
    }
}