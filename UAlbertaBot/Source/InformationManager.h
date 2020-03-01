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

    std::map<BWAPI::Player, UnitData> m_unitData;

    void updateUnit(BWAPI::Unit unit);
    void updateUnitInfo();
    bool isValidUnit(BWAPI::Unit unit);

    InformationManager();

public:

    void update();

    // event driven stuff
    void onUnitShow(BWAPI::Unit unit)       { updateUnit(unit); }
    void onUnitHide(BWAPI::Unit unit)       { updateUnit(unit); }
    void onUnitCreate(BWAPI::Unit unit)     { updateUnit(unit); }
    void onUnitComplete(BWAPI::Unit unit)   { updateUnit(unit); }
    void onUnitMorph(BWAPI::Unit unit)      { updateUnit(unit); }
    void onUnitRenegade(BWAPI::Unit unit)   { updateUnit(unit); }
    void onUnitDestroy(BWAPI::Unit unit);

    int  getNumUnits(BWAPI::UnitType type, BWAPI::Player player);
    bool isCombatUnit(BWAPI::UnitType type) const;
    void getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::Player player, int radius);
    bool enemyHasCloakedUnits();
    void drawExtendedInterface();
    void drawUnitInformation(int x, int y);
    void drawMapInformation();

    const UIMap &    getUnitInfo(BWAPI::Player player) const;
    const UnitData & getUnitData(BWAPI::Player player) const;
};
}
