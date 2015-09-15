#pragma once

#include "Common.h"
#include "MetaType.h"

namespace UAlbertaBot
{

class BuildOrder
{
    BWAPI::Race               _race;
    std::vector<MetaType>     _buildOrder;

public:

    BuildOrder();
    BuildOrder(const BWAPI::Race & race);
    BuildOrder(const BWAPI::Race & race, const std::vector<MetaType> & metaVector);

    void add(const MetaType & t);

    const size_t size() const;
    const BWAPI::Race & getRace() const;

    const MetaType & operator [] (const size_t & index) const;
    MetaType & operator [] (const size_t & index);
};

}