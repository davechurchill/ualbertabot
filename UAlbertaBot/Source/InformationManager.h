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
    std::vector<std::pair<int, BWAPI::Position>> m_scans;

    bool m_rushIncoming = false;
    bool m_enemyExpanding = false;
    bool m_enemyInBase = false;
    bool m_dtRush = false;
    bool m_turtle = false;
    int m_turtleWhen = 0;
    bool m_cloackedUnits = false;

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

    bool getRushInfo();
    void setRushInfo(bool rush);
    bool getExpandInfo();
    void setExpandInfo(bool rush);
    bool getDtRushInfo();
    void setDtRushInfo(bool expand);
    void turtleMode(bool mod);
    bool getMode();
    void setEnemyInsideInfo(bool check);
    bool getEnemyInsideInfo();
    int getModeWhen();
    int  getNumUnits(BWAPI::UnitType type, BWAPI::Player player);
    bool isCombatUnit(BWAPI::UnitType type) const;
    void getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::Player player, int radius);
    bool enemyHasCloakedUnits();
    void removeOldScans();
    bool shouldScan(BWAPI::Position castPosition);
    void drawExtendedInterface();
    void drawUnitInformation(int x, int y);
    void drawMapInformation();

    const UIMap &    getUnitInfo(BWAPI::Player player) const;
    const UnitData & getUnitData(BWAPI::Player player) const;
};
}
