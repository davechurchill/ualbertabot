#include "BaseStrategyExecutor.h"

void AKBot::BaseStrategyExecutor::loadOpeningBuildOrder(const BuildOrder & buildOrder)
{
	_buildOrder = buildOrder;
}

const UAlbertaBot::BuildOrder & AKBot::BaseStrategyExecutor::getOpeningBuildOrder() const
{
	return _buildOrder;
}
