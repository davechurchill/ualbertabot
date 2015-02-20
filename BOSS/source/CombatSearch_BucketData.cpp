#include "CombatSearch_BucketData.h"

using namespace BOSS;


// Combat Search Bucketing
// 
// Computes and stores the build order which maximizes an evaluation function up to a given time interval [t0-t1]
// The number of buckets and the frame limit determine the size of the buckets


CombatSearch_BucketData::CombatSearch_BucketData(const FrameCountType frameLimit, const size_t numBuckets)
        : _buckets(numBuckets, BucketData())
        , _frameLimit(frameLimit)
{
    
}

const size_t CombatSearch_BucketData::numBuckets() const
{
    return _buckets.size();
}

const size_t CombatSearch_BucketData::getBucketIndex(const GameState & state) const
{
    return (size_t)(((double)state.getCurrentFrame() / (double)_frameLimit) * _buckets.size());
}

void CombatSearch_BucketData::update(const GameState & state, const BuildOrder & buildOrder)
{
    if (state.getCurrentFrame() >= _frameLimit)
    {
        return;
    }

    BOSS_ASSERT(state.getCurrentFrame() <= _frameLimit, "State's frame exceeds bucket frame limit: (%d %d)", (int)state.getCurrentFrame(), (int)_frameLimit);

    // get the bucket index corresponding to the time of the current state finishing
    size_t bucketIndex = getBucketIndex(state);

    // evaluate the state with whatever value we want
    double eval = Eval::ArmyTotalResourceSum(state);

    // update the data if we have a new best value for this bucket
    BucketData & bucket = _buckets[bucketIndex];
    if ((eval > bucket.eval) || ((eval == bucket.eval) && Eval::BuildOrderBetter(buildOrder, bucket.buildOrder)))
    {
        // update every bucket for which this is a new record
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

bool CombatSearch_BucketData::isDominated(const GameState & state)
{
    return Eval::StateDominates(getBucketData(state).state, state);
}

BucketData & CombatSearch_BucketData::getBucketData(const GameState & state)
{
    BOSS_ASSERT(getBucketIndex(state) < _buckets.size(), "State goes over bucket limit");

    return _buckets[getBucketIndex(state)];
}

void CombatSearch_BucketData::print() const
{
    std::cout << "\n\nFinal CombatBucket results\n";
    std::cout << "\n  Frame     Sec     ArmyEval   BuildOrder\n";
    double maxEval = 0;

    for (size_t b(0); b<_buckets.size(); ++b)
    {
        if (_buckets[b].eval > maxEval)
        {
            maxEval = _buckets[b].eval;

            double frame = ((double)b / _buckets.size()) * _frameLimit;
            double sec   = frame / 24;

            printf("%7d %7d %12.2lf   ", (int)frame, (int)sec, _buckets[b].eval/Constants::RESOURCE_SCALE);
            std::cout << _buckets[b].buildOrder.getNameString(2) << std::endl;
        }
    }
}

const BucketData & CombatSearch_BucketData::getBucket(const size_t index) const
{
    return _buckets[index];
}

std::string CombatSearch_BucketData::getBucketResultsString()
{
    std::stringstream ss;

    ss << "0 0" << std::endl;

    double maxEval = 0;
    for (size_t b(0); b<_buckets.size(); ++b)
    {
        if (_buckets[b].eval > maxEval)
        {
            maxEval = _buckets[b].eval;

            double frame = ((double)b / _buckets.size()) * _frameLimit;
            double sec   = frame / 24;

            ss << frame << " " << _buckets[b].eval/Constants::RESOURCE_SCALE << std::endl;
        }
    }

    return ss.str();
}