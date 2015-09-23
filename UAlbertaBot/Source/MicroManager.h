#pragma once

#include "Common.h"
#include "MapGrid.h"
#include "SquadOrder.h"
#include "MapTools.h"
#include "InformationManager.h"
#include "Micro.h"

namespace UAlbertaBot
{
struct AirThreat
{
	BWAPI::UnitInterface*	unit;
	double			weight;
};

struct GroundThreat
{
	BWAPI::UnitInterface*	unit;
	double			weight;
};

class MicroManager
{
	BWAPI::Unitset  _units;

protected:
	
	SquadOrder			order;

	virtual void        executeMicro(const BWAPI::Unitset & targets) = 0;
	bool                checkPositionWalkable(BWAPI::Position pos);
	void                drawOrderText();
	bool                unitNearEnemy(BWAPI::UnitInterface* unit);
	bool                unitNearChokepoint(BWAPI::UnitInterface* unit) const;
	void                trainSubUnits(BWAPI::UnitInterface* unit) const;
    

public:
						MicroManager();
    virtual				~MicroManager(){}

	const BWAPI::Unitset & getUnits() const;
	BWAPI::Position     calcCenter() const;

	void				setUnits(const BWAPI::Unitset & u);
	void				execute(const SquadOrder & order);
	void				regroup(const BWAPI::Position & regroupPosition) const;

};
}