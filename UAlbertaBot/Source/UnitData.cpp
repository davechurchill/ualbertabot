#include "Common.h"
#include "UnitData.h"

using namespace UAlbertaBot;

UnitData::UnitData() 
	: mineralsLost(0)
	, gasLost(0)
{
	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}

	numDeadUnits	= std::vector<int>(maxTypeID + 1, 0);
	numUnits		= std::vector<int>(maxTypeID + 1, 0);
	numCompletedUnits= std::vector<int>(maxTypeID + 1, 0);
}

void UnitData::updateUnit(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }
	
	// if the unit exists, update it
	if (unitMap.find(unit) != unitMap.end())
	{
		UnitInfo & ui = unitMap.find(unit)->second;

		ui.lastPosition = unit->getPosition();
		ui.lastHealth = unit->getHitPoints();
        ui.lastShields= unit->getShields();
		ui.unitID = unit->getID();
		ui.type = unit->getType();
		if (!ui.completed&&unit->isCompleted()){
			numCompletedUnits[unit->getType().getID()]++;
		}
        ui.completed = unit->isCompleted();

		return;
	}
	// otherwise create it
	else
	{
		// put the unit in the map
		numUnits[unit->getType().getID()]++;
		if (unit->isCompleted()){
			numCompletedUnits[unit->getType().getID()]++;
		}
		unitMap[unit] = UnitInfo(unit->getID(), unit, unit->getPosition(), unit->getType());
	}
}

void UnitData::removeUnit(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }

	mineralsLost += unit->getType().mineralPrice();
	gasLost += unit->getType().gasPrice();
	numUnits[unit->getType().getID()]--;
	numCompletedUnits[unit->getType().getID()]--;
	numDeadUnits[unit->getType().getID()]++;
		
	unitMap.erase(unit);
}

void UnitData::removeBadUnits()
{
	// for each unit in the map
	for (UIMapIter iter(unitMap.begin()); iter != unitMap.end();)
	{
		// if it is bad
		if (badUnitInfo(iter->second))
		{
			// remove it from the map
			numUnits[iter->second.type.getID()]--;
			numCompletedUnits[iter->second.type.getID()]--;
			iter = unitMap.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

const bool UnitData::badUnitInfo(const UnitInfo & ui) const
{
	// Cull away any refineries/assimilators/extractors that were destroyed and reverted to vespene geysers
	if(ui.unit && ui.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{
		return true;
	}

	// If the unit is a building and we can currently see its position and it is not there
	if(ui.type.isBuilding() && BWAPI::Broodwar->isVisible(ui.lastPosition.x/32, ui.lastPosition.y/32) && !ui.unit->isVisible())
	{
		return true;
	}

	return false;
}

void UnitData::getCloakedUnits(std::set<UnitInfo> & v) const 
{
	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.canCloak())
		{
			v.insert(ui);
		}
	}
}

void UnitData::getDetectorUnits(std::set<UnitInfo> & v) const 
{
	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.isDetector())
		{
			v.insert(ui);
		}
	}
}

void UnitData::getFlyingUnits(std::set<UnitInfo> & v) const 
{
	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.isFlyer())
		{
			v.insert(ui);
		}
	}
}

bool UnitData::hasCloakedUnits() const	
{ 
	if (numUnits[BWAPI::UnitTypes::Protoss_Citadel_of_Adun] > 0)
	{
		return true;
	}

	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.canCloak())
		{
			return true;
		}
	}

	return false;
}

int UnitData::numCloakedUnits()	const
{
	int count = 0;
	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.canCloak())
		{
			count++;
		}
	}

	return count;
}

bool UnitData::hasDetectorUnits() const 
{ 
	FOR_EACH_UIMAP_CONST(iter, unitMap)
	{
		const UnitInfo & ui(iter->second);

		if (ui.isDetector())
		{
			return true;
		}
	}

	return false;
}