#include "TransportManager.h"

using namespace UAlbertaBot;

TransportManager::TransportManager()  { }

void TransportManager::executeMicro(const UnitVector & targets) 
{
	const UnitVector & transportUnits = getUnits();

	if (transportUnits.empty())
	{
		return;
	}
}