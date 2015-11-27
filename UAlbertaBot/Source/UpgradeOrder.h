#pragma once

#include "Common.h"
#include "MetaType.h"

namespace UAlbertaBot
{

class UpgradeOrder
{
    BWAPI::Race               _race;
    std::vector<MetaType>     _upgradeOrder;
	mutable size_t			  _currentUpgrade = 0;

public:

    UpgradeOrder();
	UpgradeOrder(const BWAPI::Race & race);
	UpgradeOrder(const BWAPI::Race & race, const std::vector<MetaType> & metaVector);
	
	MetaType getNextUpgrade();
	void	 upgradeAddedToBuild();

    void add(const MetaType & t);

    const size_t size() const;
    const BWAPI::Race & getRace() const;

    const MetaType & operator [] (const size_t & index) const;
    MetaType & operator [] (const size_t & index);
};

}