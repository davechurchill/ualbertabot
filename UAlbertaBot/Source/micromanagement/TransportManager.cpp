#include "TransportManager.h"

using namespace UAlbertaBot;

TransportManager::TransportManager()  { }

void TransportManager::executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) 
{
	const std::vector<BWAPI::UnitInterface *> & transportUnits = getUnits();

	if (transportUnits.empty())
	{
		return;
	}
}