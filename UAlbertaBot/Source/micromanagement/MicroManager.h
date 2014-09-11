#pragma once

#include "Common.h"
#include "../MapGrid.h"
#include "../SquadOrder.h"
#include "../MapTools.h"
#include "../InformationManager.h"

namespace UAlbertaBot
{
struct AirThreat
{
	BWAPI::Unit *	unit;
	double			weight;
};

struct GroundThreat
{
	BWAPI::Unit *	unit;
	double			weight;
};

class MicroManager
{
	UnitVector			units;
	int					lastRegroupPerformed;

protected:
	
	SquadOrder			order;

	virtual void		executeMicro(const UnitVector & targets) = 0;
	bool				checkPositionWalkable(BWAPI::Position pos);
	bool				drawDebugVectors;
	void				drawOrderText();
	void				smartAttackUnit(BWAPI::Unit * attacker, BWAPI::Unit * target) const;
	void				smartAttackMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition) const;
	void				smartMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition) const;
	bool				unitNearEnemy(BWAPI::Unit * unit);
	bool				unitNearChokepoint(BWAPI::Unit * unit) const;
	void				trainSubUnits(BWAPI::Unit * unit) const;

public:
						MicroManager() : drawDebugVectors(true), lastRegroupPerformed(0) {}
    virtual				~MicroManager(){}

	const UnitVector &	getUnits() const { return units; }
	BWAPI::Position     calcCenter() const;

	void				setUnits(const UnitVector & u);
	void				execute(const SquadOrder & order);
	void				regroup(const BWAPI::Position & regroupPosition) const;
};
}