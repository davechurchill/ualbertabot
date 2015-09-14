#pragma once

#include "Common.h"
#include "HLUnitData.h"

namespace UAlbertaBot{
	class HLSquadOrder
	{
	public:

		enum OrderType { None, Attack, Defend, SquadOrderTypes };

		OrderType			_type;
		BWTA::Region*		_target;

		HLSquadOrder();

		HLSquadOrder(OrderType type, BWTA::Region* target);

	};

	class HLSquad{
		BWTA::Region *_currentRegion;
		std::list<BWTA::Region*>  _path;
		int _framesTravelled;
		std::unordered_map<int,UnitInfo> _units;
		HLSquadOrder _order;
		double _speed;
		mutable int _groundStrength;
		mutable int _flyingStrength;
		mutable int _antiAirStrength;
		mutable int _strength;
		mutable bool _strengthCalculated;
	public:
		HLSquad(const std::vector<UnitInfo> & units, BWTA::Region *region);
		~HLSquad(){};
		void addUnit(const UnitInfo &unit);
		double getSpeed() const{ return _speed; }
		bool hasPath() const{ return !_path.empty(); }
		BWTA::Region* getNextRegion() const{ return _path.front(); }
		BWTA::Region* getCurrentRegion() const{ return _currentRegion; }
		int getTravelledTime() const{ return _framesTravelled; }
		int travel(int frames);
		int getGroundStrength() const;
		int getStrength() const;
		int getAntiAirStrength() const;
		int getFlyingStrength() const;
		void calculateStrength() const;
		void order(HLSquadOrder order);
		HLSquadOrder order() const{ return _order; }
		//std::vector<UnitInfo>& getUnits()  { return _units; }
		//const std::vector<UnitInfo>& getUnits() const { return _units; }
		void removeUnit(const UnitInfo &unit){ _units.erase(unit.unitID);  }
		void removeUnit(int id){ _units.erase(id); _strengthCalculated = false; }
		//void removeUnit(const std::unordered_set<UnitInfo>::iterator &unit){ _units.erase(unit); _strengthCalculated = false; }
		//void updateUnit(const UnitInfo &unit)
		//{ 
		//	removeUnit(unit);
		//	addUnit(unit);
		//}
		std::unordered_map<int,UnitInfo>::iterator begin(){ return _units.begin(); }
		std::unordered_map<int, UnitInfo>::iterator end(){ return _units.end(); }
		std::unordered_map<int, UnitInfo>::const_iterator begin() const{ return _units.begin(); }
		std::unordered_map<int, UnitInfo>::const_iterator end() const{ return _units.end(); }
		UnitInfo& operator[](const int &id){ return _units[id]; }
		UnitInfo& at(const int &id){ return _units.at(id); }
		bool empty() const{ return _units.empty(); }
		std::string toString() const;
		//HLSquad(const Squad &squad);//implicit conversion
		//bool done();//check if order completed
		//BWAPI::Region getRegion() const{ return region; }
		//void addUnit(BWAPI::UnitInterface *u){ Squad::addUnit(u); }
		//bool removeUnit(BWAPI::UnitInterface *u){ return Squad::removeUnit(u); }
	};
	std::list<BWTA::Region*> getPath(BWTA::Region *from, BWTA::Region *to);
	std::unordered_set<BWTA::Region*> getNeighbours(const BWTA::Region *region);
}