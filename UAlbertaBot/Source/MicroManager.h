#pragma once

#include "Common.h"
#include "SquadOrder.h"
#include "Micro.h"
#include "BaseLocationManager.h"
#include "ScreenCanvas.h"

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
	const BaseLocationManager& _bases;
	const AKBot::OpponentView& _opponentView;

protected:
	
	SquadOrder			order;

	virtual void        executeMicro(const std::vector<BWAPI::Unit> & targets) = 0;
	bool                checkPositionWalkable(BWAPI::Position pos);
	void                trainSubUnits(BWAPI::Unit unit) const;
    

public:
						MicroManager(const AKBot::OpponentView& opponentView, const BaseLocationManager& bases);

	const std::vector<BWAPI::Unit> & getUnits() const;

	void				setUnits(const std::vector<BWAPI::Unit> & u);
	void				execute(const MapTools & map, const SquadOrder & order);
	void				regroup(const MapTools & map, const BWAPI::Position & regroupPosition) const;
	void                drawOrderText(AKBot::ScreenCanvas& canvas, const SquadOrder & order);

};
}