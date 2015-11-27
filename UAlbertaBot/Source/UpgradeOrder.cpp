#include "UpgradeOrder.h"

using namespace UAlbertaBot;

UpgradeOrder::UpgradeOrder()
    : _race(BWAPI::Races::None)
{

}

UpgradeOrder::UpgradeOrder(const BWAPI::Race & race)
    : _race(race)
{

}

UpgradeOrder::UpgradeOrder(const BWAPI::Race & race, const std::vector<MetaType> & metaVector)
    : _race(race)
    , _upgradeOrder(metaVector)
{

}

void UpgradeOrder::add(const MetaType & t)
{
    UAB_ASSERT(t.getRace() == getRace(), "Trying to add difference Race metatype to upgrade order");
	UAB_ASSERT(t.isUpgrade(), "Trying to add something that is not an upgrade");

	// Place at front
	_upgradeOrder.insert(_upgradeOrder.begin(),t);
}

const BWAPI::Race & UpgradeOrder::getRace() const
{
    return _race;
}

const size_t UpgradeOrder::size() const
{
	return _upgradeOrder.size();
}

bool UpgradeOrder::empty() const
{
	return _upgradeOrder.empty();
}

MetaType UpgradeOrder::getNextUpgrade() {
	return _upgradeOrder.back();
}


void	 UpgradeOrder::upgradeAddedToBuild() {
	if (!_upgradeOrder.empty()){
		_upgradeOrder.pop_back();
	}
}