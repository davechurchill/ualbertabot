#include "HLUnitData.h"
using namespace UAlbertaBot;

HLUnitData::HLUnitData() :_highestID(-1)
{
}

HLUnitData::HLUnitData(const UnitData &data) :UnitData(data)
{
	_highestID = -1;
	for (auto unit : getUnits()){
		if (unit.second.unitID > _highestID){
			_highestID = unit.second.unitID;
		}
	}
}

HLUnitData::~HLUnitData()
{
}


//virtual methods
void	HLUnitData::getCloakedUnits(std::set<UnitInfo> & v)				const
{
	UnitData::getCloakedUnits(v);

	for (auto item : _fakeUnitMap)
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
	int count = UnitData::hasCloakedUnits();
	for (auto item : _fakeUnitMap)
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
	UnitData::getDetectorUnits(v);

	for (auto item : _fakeUnitMap)
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
	UnitData::getFlyingUnits(v);

	for (auto item : _fakeUnitMap)
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
	return UnitData::hasCloakedUnits() || [this](){
		for (auto item : _fakeUnitMap)
		{
			const UnitInfo & ui(item.second);

			if (ui.canCloak())
			{
				return true;
			}
		}
		return false;
	}();
}

bool	HLUnitData::hasDetectorUnits()									const
{
	return UnitData::hasDetectorUnits() || [this](){
		for (auto item : _fakeUnitMap)
		{
			const UnitInfo & ui(item.second);

			if (ui.isDetector())
			{
				return true;
			}
		}
		return false;
	}();
}

//int		HLUnitData::getNumUnits(BWAPI::UnitType t)						const
//{
//}
//
//int		HLUnitData::getNumCompletedUnits(BWAPI::UnitType t)				const
//{
//}
//
//int		HLUnitData::getNumDeadUnits(BWAPI::UnitType t)					const
//{
//}

//new methods
void HLUnitData::removeUnit(int id)
{
	if (_fakeUnitMap.find(id) != _fakeUnitMap.end()){
		const UnitInfo &unit = _fakeUnitMap.at(id);
		numDeadUnits[unit.type.getID()]++;
		numCompletedUnits[unit.type.getID()]--;
		numUnits[unit.type.getID()]--;
		_fakeUnitMap.erase(id);
	}

	//todo:decrease highest ID?
}

void HLUnitData::addUnit(const UnitInfo &unit)
{
	if (_fakeUnitMap.find(unit.unitID) == _fakeUnitMap.end()){
		_fakeUnitMap[unit.unitID] = unit;
		if (unit.completed)numCompletedUnits[unit.type.getID()]++;
		numUnits[unit.type.getID()]++;
		if (unit.unitID > _highestID){
			_highestID = unit.unitID;
		}
	}
}

std::vector<UnitInfo> HLUnitData::getUnitVector()						const
{
	std::vector<UnitInfo> v;
	std::transform(getUnits().begin(), getUnits().end(), std::back_inserter(v), 
		[](const std::pair<BWAPI::UnitInterface*, UnitInfo> &item){
		return item.second;
	});
	std::transform(_fakeUnitMap.begin(), _fakeUnitMap.end(), std::back_inserter(v),
		[](const std::pair<int, UnitInfo> &item){
		return item.second;
	});
	return v;
}
