#include "BuildOrder.h"

using namespace UAlbertaBot;

BuildOrder::BuildOrder()
    : _race(BWAPI::Races::None)
{

}

BuildOrder::BuildOrder(const BWAPI::Race & race)
    : _race(race)
{

}

BuildOrder::BuildOrder(const BWAPI::Race & race, const std::vector<MetaType> & metaVector)
    : _race(race)
    , _buildOrder(metaVector)
{

}

void BuildOrder::add(const MetaType & t)
{
    UAB_ASSERT(t.getRace() == getRace(), "Trying to add difference Race metatype to build order");

    _buildOrder.push_back(t);
}

const BWAPI::Race & BuildOrder::getRace() const
{
    return _race;
}

const size_t BuildOrder::size() const
{
    return _buildOrder.size();
}

const MetaType & BuildOrder::operator [] (const size_t & index) const
{
    return _buildOrder[index];
}

MetaType & BuildOrder::operator [] (const size_t & index)
{
    return _buildOrder[index];
}