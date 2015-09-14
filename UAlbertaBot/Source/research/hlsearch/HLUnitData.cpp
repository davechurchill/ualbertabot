#include "HLUnitData.h"
using namespace UAlbertaBot;

HLUnitData::HLUnitData() :_highestID(-1), mineralsLost(0), gasLost(0), _player(NULL)
{
	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}

	numDeadUnits = std::vector<int>(maxTypeID + 1, 0);
	numUnits = std::vector<int>(maxTypeID + 1, 0);
	numCompletedUnits = std::vector<int>(maxTypeID + 1, 0);
}
HLUnitData::HLUnitData(BWAPI::Player player) :HLUnitData()
{
	_player = player;
}

HLUnitData::HLUnitData(const UnitData &data, BWAPI::Player player) : HLUnitData(player)
{
	for (auto unit : data.getUnits()){
		addUnit(unit.second);
	}
	mineralsLost = data.getMineralsLost();
	gasLost = data.getGasLost();
}

HLUnitData::~HLUnitData()
{
}


//virtual methods
void	HLUnitData::getCloakedUnits(std::set<UnitInfo> & v)				const
{

	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.canCloak())
		{
			v.insert(ui);
		}
	}
}

int		HLUnitData::numCloakedUnits()									const
{
	int count = 0;
	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.canCloak())
		{
			count++;
		}
	}
	return count;
}

void	HLUnitData::getDetectorUnits(std::set<UnitInfo> & v)			const
{


	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.isDetector())
		{
			v.insert(ui);
		}
	}
}

void	HLUnitData::getFlyingUnits(std::set<UnitInfo> & v)				const
{


	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.isFlyer())
		{
			v.insert(ui);
		}
	}
}

bool	HLUnitData::hasCloakedUnits()									const
{

	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.canCloak())
		{
			return true;
		}
	}
	return false;

}

bool	HLUnitData::hasDetectorUnits()									const
{

	for (auto item : _unitMap)
	{
		const UnitInfo & ui(item.second);

		if (ui.isDetector())
		{
			return true;
		}
	}
	return false;

}

int		HLUnitData::getNumUnits(BWAPI::UnitType t)						const
{
	return numUnits[t.getID()];
}

int		HLUnitData::getNumCompletedUnits(BWAPI::UnitType t)				const
{
	return numCompletedUnits[t.getID()];
}

int		HLUnitData::getNumDeadUnits(BWAPI::UnitType t)					const
{
	return numDeadUnits[t.getID()];
}

//new methods
void HLUnitData::removeUnit(int id)
{
	if (_unitMap.find(id) != _unitMap.end()){
		const UnitInfo &unit = _unitMap.at(id);
		numDeadUnits[unit.type.getID()]++;
		numCompletedUnits[unit.type.getID()]--;
		numUnits[unit.type.getID()]--;
		_unitMap.erase(id);
	}

	//todo:decrease highest ID?
	//todo:update _baseRegions?
}

void HLUnitData::addUnit(const UnitInfo &unit)
{
	if (_unitMap.find(unit.unitID) == _unitMap.end()){
		_unitMap[unit.unitID] = unit;
		if (unit.completed){
			numCompletedUnits[unit.type.getID()]++;
		}
		numUnits[unit.type.getID()]++;
		if (unit.unitID > _highestID){
			_highestID = unit.unitID;
		}
	}
	else{//update info
		_unitMap[unit.unitID].completed = unit.completed;
		_unitMap[unit.unitID].lastHealth = unit.lastHealth;
		_unitMap[unit.unitID].lastPosition = unit.lastPosition;
	}
	if (unit.type.isBuilding())
	{
		try
		{
			_baseRegions.insert(BWTA::getRegion(unit.lastPosition));
		}
		catch (...)
		{
			Logger::LogAppendToFile(UAB_LOGFILE, "Exception at BWTA::getRegion(%d, %d), ignoring unit", unit.lastPosition.x, unit.lastPosition.y);
		}
	}
}

bool HLUnitData::finishUnit(BWAPI::UnitType type)
{
	bool found = false;
	for (auto &item : _unitMap){
		if (item.second.type.getID() == type.getID() && !item.second.completed){
			item.second.completed = true;
			numCompletedUnits[type.getID()]++;
			found = true;
			break;
		}
	}

	UAB_ASSERT(found,"Couldn't find unit to finish: %s\n",type.getName().c_str());
	return found;
}

//std::vector<UnitInfo> HLUnitData::getUnitVector()						const
//{
//	std::vector<UnitInfo> v;
//	std::transform(_unitMap.begin(), _unitMap.end(), std::back_inserter(v),
//		[](const std::pair<int, UnitInfo> &item){
//		return item.second;
//	});
//	return v;
//}
