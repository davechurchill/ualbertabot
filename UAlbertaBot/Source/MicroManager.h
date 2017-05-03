#pragma once

#include "Common.h"
#include "SquadOrder.h"
#include "Micro.h"

namespace UAlbertaBot
{
struct AirThreat
{
	BWAPI::Unit	unit;
	double weight;
};

struct GroundThreat
{
	BWAPI::Unit	unit;
	double weight;
};

class MicroManager
{
	BWAPI::Unitset _units;

protected:
	
	SquadOrder			order;

	virtual void        executeMicro(const BWAPI::Unitset & targets) = 0;
	bool                checkPositionWalkable(BWAPI::Position pos);
	void                drawOrderText();
	void                trainSubUnits(BWAPI::Unit unit) const;
    

public:
						MicroManager();

	const BWAPI::Unitset & getUnits() const;

	void				setUnits(const BWAPI::Unitset & u);
	void				execute(const SquadOrder & order);
	void				regroup(const BWAPI::Position & regroupPosition) const;

};
}