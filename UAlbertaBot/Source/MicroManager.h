#pragma once

#include "Common.h"
#include "MapGrid.h"
#include "SquadOrder.h"
#include "MapTools.h"
#include "InformationManager.h"

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
	std::vector<BWAPI::UnitInterface *>			units;
	int					lastRegroupPerformed;

protected:
	
	SquadOrder			order;

	virtual void		executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) = 0;
	bool				checkPositionWalkable(BWAPI::Position pos);
	bool				drawDebugVectors;
	void				drawOrderText();
	void				smartAttackUnit(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target) const;
	void				smartAttackMove(BWAPI::UnitInterface* attacker, BWAPI::Position targetPosition) const;
	void				smartMove(BWAPI::UnitInterface* attacker, BWAPI::Position targetPosition) const;
	bool				unitNearEnemy(BWAPI::UnitInterface* unit);
	bool				unitNearChokepoint(BWAPI::UnitInterface* unit) const;
	void				trainSubUnits(BWAPI::UnitInterface* unit) const;

public:
						MicroManager() : drawDebugVectors(true), lastRegroupPerformed(0) {}
    virtual				~MicroManager(){}

	const std::vector<BWAPI::UnitInterface *> &	getUnits() const { return units; }
	BWAPI::Position     calcCenter() const;

	void				setUnits(const std::vector<BWAPI::UnitInterface *> & u);
	void				execute(const SquadOrder & order);
	void				regroup(const BWAPI::Position & regroupPosition) const;
};
}