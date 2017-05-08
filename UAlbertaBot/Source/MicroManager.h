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
	std::vector<BWAPI::Unit> _units;

protected:
	
	SquadOrder			order;

	virtual void        executeMicro(const std::vector<BWAPI::Unit> & targets) = 0;
	bool                checkPositionWalkable(BWAPI::Position pos);
	void                drawOrderText();
	void                trainSubUnits(BWAPI::Unit unit) const;
    

public:
						MicroManager();

	const std::vector<BWAPI::Unit> & getUnits() const;

	void				setUnits(const std::vector<BWAPI::Unit> & u);
	void				execute(const SquadOrder & order);
	void				regroup(const BWAPI::Position & regroupPosition) const;

};
}