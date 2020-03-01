#pragma once

#include "Common.h"
#include "SquadOrder.h"

namespace UAlbertaBot
{
struct AirThreat
{
    BWAPI::Unit	unit;
    double      weight;
};

struct GroundThreat
{
    BWAPI::Unit	unit;
    double      weight;
};

class MicroManager
{
    BWAPI::Unitset  m_units;

protected:

    SquadOrder m_order;

    virtual void executeMicro(const BWAPI::Unitset & targets) = 0;
    bool checkPositionWalkable(BWAPI::Position pos);
    void drawOrderText();
    bool unitNearEnemy(BWAPI::Unit unit);
    bool unitNearChokepoint(BWAPI::Unit unit) const;
    void trainSubUnits(BWAPI::Unit unit) const;

public:

    MicroManager();
    virtual ~MicroManager() {}

    const BWAPI::Unitset & getUnits() const;
    BWAPI::Position calcCenter() const;

    void setUnits(const BWAPI::Unitset & u);
    void execute(const SquadOrder & order);
    void regroup(const BWAPI::Position & regroupPosition) const;

};
}