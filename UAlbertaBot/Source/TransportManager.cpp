#include "TransportManager.h"

using namespace UAlbertaBot;

TransportManager::TransportManager()  { }

void TransportManager::executeMicro(const BWAPI::Unitset & targets) 
{
	const BWAPI::Unitset & transportUnits = getUnits();

	if (transportUnits.empty())
	{
		return;
	}
}