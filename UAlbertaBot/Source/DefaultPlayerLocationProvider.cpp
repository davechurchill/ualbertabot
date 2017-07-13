#include "DefaultPlayerLocationProvider.h"

AKBot::DefaultPlayerLocationProvider::DefaultPlayerLocationProvider(const BaseLocationManager* baseLocationManager)
	: _baseLocationManager(baseLocationManager)
{
}

const UAlbertaBot::BaseLocation * AKBot::DefaultPlayerLocationProvider::getPlayerStartingBaseLocation(BWAPI::Player player)
{
	return _baseLocationManager->getPlayerStartingBaseLocation(player);
}
