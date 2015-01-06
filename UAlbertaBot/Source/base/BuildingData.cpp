#include "BuildingData.h"

using namespace UAlbertaBot;

ConstructionData::ConstructionData()
{
	// set up the buildings vector
	buildings = std::vector< std::vector<Building> > (NumBuildingStates, std::vector<Building>());

	// set up the index vector
	buildingIndex = std::vector< size_t >(NumBuildingStates, 0);
}

bool ConstructionData::hasNextBuilding(BuildingState bs)
{
	// is the current index valid
	return buildingIndex[bs] < buildings[bs].size();
}

int ConstructionData::getNumBuildings(BuildingState bs)
{
	return buildings[bs].size();
}

Building & ConstructionData::getNextBuilding(BuildingState bs)
{
	// assert this index is valid
	assert(hasNextBuilding(bs));

	// get the building
	Building & returnBuilding = buildings[bs][buildingIndex[bs]];

	// increment the index
	buildingIndex[bs]++;

	// return the building
	return returnBuilding;
}

void ConstructionData::begin(BuildingState bs)
{
	// set the index to 0
	buildingIndex[bs] = 0;
}

void ConstructionData::removeCurrentBuilding(BuildingState bs)
{
	// erase the element of the vector 1 before where we are now
	buildings[bs].erase(buildings[bs].begin() + buildingIndex[bs] - 1);

	// set the index back one
	buildingIndex[bs]--;
}

void ConstructionData::addBuilding(BuildingState bs, const Building & b)
{
	// add the building to the vector
	buildings[bs].push_back(b);
}

bool ConstructionData::isBeingBuilt(BWAPI::UnitType type)
{
	// for each building vector
	for (std::vector<Building> & buildingVector : buildings)
	{
		for (Building & b : buildingVector)
		{
			if (b.type == type)
			{
				return true;
			}
		}
	}

	return false;
}