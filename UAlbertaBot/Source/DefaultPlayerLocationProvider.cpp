#include "DefaultPlayerLocationProvider.h"

AKBot::DefaultPlayerLocationProvider::DefaultPlayerLocationProvider(shared_ptr<BaseLocationManager> baseLocationManager)
	: _baseLocationManager(baseLocationManager)
{
}

const UAlbertaBot::BaseLocation * AKBot::DefaultPlayerLocationProvider::getPlayerStartingBaseLocation(BWAPI::Player player) const
{
	return _baseLocationManager->getPlayerStartingBaseLocation(player);
}
