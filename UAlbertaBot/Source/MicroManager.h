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

class MapTools;

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
	void				execute(const MapTools & map, const SquadOrder & order);
	void				regroup(const MapTools & map, const BWAPI::Position & regroupPosition) const;

};
}