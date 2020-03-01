#include "BuildOrder.h"

using namespace UAlbertaBot;

BuildOrder::BuildOrder()
{

}

BuildOrder::BuildOrder(const BWAPI::Race & race)
    : m_race(race)
{

}

BuildOrder::BuildOrder(const BWAPI::Race & race, const std::vector<MetaType> & metaVector)
    : m_race(race)
    , m_buildOrder(metaVector)
{

}

void BuildOrder::add(const MetaType & t)
{
    UAB_ASSERT(t.getRace() == getRace(), "Trying to add difference Race metatype to build order");

    m_buildOrder.push_back(t);
}

const BWAPI::Race & BuildOrder::getRace() const
{
    return m_race;
}

const size_t BuildOrder::size() const
{
    return m_buildOrder.size();
}

const MetaType & BuildOrder::operator [] (const size_t & index) const
{
    return m_buildOrder[index];
}

MetaType & BuildOrder::operator [] (const size_t & index)
{
    return m_buildOrder[index];
}