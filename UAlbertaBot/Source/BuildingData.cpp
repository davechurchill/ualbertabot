#include "BuildingData.h"

using namespace UAlbertaBot;

BuildingData::BuildingData()
{

}

void BuildingData::removeBuilding(const Building & b)
{
    auto & building = std::find(m_buildings.begin(), m_buildings.end(), b);

    if (building != m_buildings.end())
    {
        m_buildings.erase(building);
    }
}

std::vector<Building> & BuildingData::getBuildings()
{
    return m_buildings;
}

void BuildingData::addBuilding(const Building & b)
{
    m_buildings.push_back(b);
}

bool BuildingData::isBeingBuilt(BWAPI::UnitType type)
{
    for (auto & b : m_buildings)
    {
        if (b.type == type)
        {
            return true;
        }
    }

    return false;
}

void BuildingData::removeBuildings(const std::vector<Building> & buildings)
{
    for (const auto & b : buildings)
    {
        removeBuilding(b);
    }
}