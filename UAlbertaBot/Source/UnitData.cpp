#include "Common.h"
#include "UnitData.h"
#include <BWAPI/Game.h>

using namespace UAlbertaBot;

UnitData::UnitData() 
	: _mineralsLost(0)
	, _gasLost(0)
{
	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}

	_numDeadUnits	    = std::vector<int>(maxTypeID + 1, 0);
	_numUnits		    = std::vector<int>(maxTypeID + 1, 0);
}

void UnitData::updateUnit(BWAPI::Unit unit)
{
	if (unit == nullptr) 
    { 
        return; 
    }

    bool firstSeen = false;
    auto & it = _unitMap.find(unit->getID());
    if (it == _unitMap.end())
    {
        firstSeen = true;
        _unitMap[unit->getID()] = UnitInfo();
    }
    
	UnitInfo & ui   = _unitMap[unit->getID()];
    ui.unit         = unit;
    ui.player       = unit->getPlayer();
	ui.lastPosition = unit->getPosition();
	ui.lastHealth   = unit->getHitPoints();
    ui.lastShields  = unit->getShields();
	ui.unitID       = unit->getID();
	ui.type         = unit->getType();
    ui.completed    = unit->isCompleted();

    if (firstSeen)
    {
        _numUnits[unit->getType().getID()]++;
    }
}

void UnitData::removeUnit(BWAPI::Unit unit)
{
	if (unit == nullptr) 
    { 
        return; 
    }

	_mineralsLost += unit->getType().mineralPrice();
	_gasLost += unit->getType().gasPrice();
	_numUnits[unit->getType().getID()]--;
	_numDeadUnits[unit->getType().getID()]++;
		
	_unitMap.erase(unit->getID());
}

void UnitData::removeBadUnits()
{
	for (auto iter = _unitMap.begin(); iter != _unitMap.end();)
	{
		if (badUnitInfo(iter->second))
		{
			_numUnits[iter->second.type.getID()]--;
			iter = _unitMap.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

const bool UnitData::badUnitInfo(const UnitInfo & ui) const
{
    if (!ui.unit)
    {
        return false;
    }

	// Cull away any refineries/assimilators/extractors that were destroyed and reverted to vespene geysers
	if (ui.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{ 
		return true;
	}

	// If the unit is a building and we can currently see its position and it is not there
	if (ui.type.isBuilding() && BWAPI::Broodwar->isVisible(ui.lastPosition.x/32, ui.lastPosition.y/32) && !ui.unit->isVisible())
	{
		return true;
	}

	return false;
}

int UnitData::getGasLost() const 
{ 
    return _gasLost; 
}

int UnitData::getMineralsLost() const 
{ 
    return _mineralsLost; 
}

int UnitData::getNumUnits(BWAPI::UnitType t) const 
{ 
    return _numUnits[t.getID()]; 
}

int UnitData::getNumDeadUnits(BWAPI::UnitType t) const 
{ 
    return _numDeadUnits[t.getID()]; 
}

const std::map<int, UnitInfo> & UnitData::getUnitInfoMap() const 
{ 
    return _unitMap; 
}