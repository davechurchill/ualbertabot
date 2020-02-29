#pragma once

#include "Common.h"
#include "UnitData.h"

namespace UAlbertaBot
{
struct BaseInfo;

typedef std::vector<BaseInfo> BaseInfoVector;

class InformationManager 
{
    friend class Global;

    InformationManager();
    
    BWAPI::Player       _self;
    BWAPI::Player       _enemy;

    std::map<BWAPI::Player, UnitData>                   _unitData;
    int                     getIndex(BWAPI::Player player) const;

    void                    updateUnit(BWAPI::Unit unit);
    void                    updateUnitInfo();
    bool                    isValidUnit(BWAPI::Unit unit);

public:

    void                    update();

    // event driven stuff
    void					onUnitShow(BWAPI::Unit unit)        { updateUnit(unit); }
    void					onUnitHide(BWAPI::Unit unit)        { updateUnit(unit); }
    void					onUnitCreate(BWAPI::Unit unit)		{ updateUnit(unit); }
    void					onUnitComplete(BWAPI::Unit unit)    { updateUnit(unit); }
    void					onUnitMorph(BWAPI::Unit unit)       { updateUnit(unit); }
    void					onUnitRenegade(BWAPI::Unit unit)    { updateUnit(unit); }
    void					onUnitDestroy(BWAPI::Unit unit);

    int						getNumUnits(BWAPI::UnitType type,BWAPI::Player player);
    bool					nearbyForceHasCloaked(BWAPI::Position p,BWAPI::Player player,int radius);
    bool					isCombatUnit(BWAPI::UnitType type) const;

    void                    getNearbyForce(std::vector<UnitInfo> & unitInfo,BWAPI::Position p,BWAPI::Player player,int radius);

    const UIMap &           getUnitInfo(BWAPI::Player player) const;
	
    bool                    enemyHasCloakedUnits();

    void                    drawExtendedInterface();
    void                    drawUnitInformation(int x,int y);
    void                    drawMapInformation();

    const UnitData &        getUnitData(BWAPI::Player player) const;
};
}
