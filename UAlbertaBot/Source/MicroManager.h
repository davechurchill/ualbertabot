#pragma once

#include "Common.h"
#include "SquadOrder.h"
#include "BaseLocationManager.h"
#include "BotConfiguration.h"

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
	const BotMicroConfiguration& _microConfiguration;
protected:
	
	SquadOrder			order;
	shared_ptr<BaseLocationManager> bases;
	shared_ptr<AKBot::OpponentView> opponentView;

	virtual void        executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame) = 0;
	bool                checkPositionWalkable(BWAPI::Position pos);
	void                trainSubUnits(BWAPI::Unit unit) const;
    

public:
	MicroManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		const BotMicroConfiguration& microConfiguration);

	const std::vector<BWAPI::Unit> & getUnits() const;

	void				setUnits(const std::vector<BWAPI::Unit> & u);
	void				execute(shared_ptr<MapTools> map, const SquadOrder & order, int currentFrame);
	void				regroup(shared_ptr<MapTools> map, const BWAPI::Position & regroupPosition, int currentFrame) const;
	const BotMicroConfiguration& configuration() const;
	void setCurrentOrder(const SquadOrder & inputOrder);
};
}