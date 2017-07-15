#pragma once

#include "Common.h"
#include "UnitData.h"
#include "BaseLocation.h"
#include "..\..\SparCraft\source\SparCraft.h"

namespace UAlbertaBot
{
struct BaseInfo;
typedef std::vector<BaseInfo> BaseInfoVector;

class UnitInfoManager 
{
    std::map<BWAPI::Player, UnitData> _unitData;
    std::map<BWAPI::Player, std::set<const BaseLocation *> >  _occupiedBaseLocations;
	const AKBot::OpponentView& _opponentView;

    void                    updateUnit(BWAPI::Unit unit);
    void                    updateUnitInfo();
    bool                    isValidUnit(BWAPI::Unit unit);

public:

    UnitInfoManager(const AKBot::OpponentView& opponentView);

    void                    update();
    void                    onStart();

    // event driven stuff
    void					onUnitShow(BWAPI::Unit unit)        { updateUnit(unit); }
    void					onUnitHide(BWAPI::Unit unit)        { updateUnit(unit); }
    void					onUnitCreate(BWAPI::Unit unit)		{ updateUnit(unit); }
    void					onUnitComplete(BWAPI::Unit unit)    { updateUnit(unit); }
    void					onUnitMorph(BWAPI::Unit unit)       { updateUnit(unit); }
    void					onUnitRenegade(BWAPI::Unit unit)    { updateUnit(unit); }
    void					onUnitDestroy(BWAPI::Unit unit);
	
	/*
		Checks whether unit is enemy unit or not.
	*/
	bool isEnemyUnit(BWAPI::Unit unit);

    void                    getNearbyForce(std::vector<UnitInfo> & unitInfo,BWAPI::Position p,BWAPI::Player player,int radius) const;

    const std::map<int, UnitInfo> & getUnitInfoMap(BWAPI::Player player) const;
    bool                    enemyHasCloakedUnits() const;
	const UnitData &        getUnitData(BWAPI::Player player) const;

};
}
