#pragma once

#include "Squad.h"
#include "Logger.h"

namespace UAlbertaBot
{
class SquadData
{
	std::map<std::string, Squad> _squads;
	UnitHandler onRemoveHandler;
	std::shared_ptr<AKBot::Logger> _logger;

    void    updateAllSquads(shared_ptr<MapTools> map, int currentFrame);
    void    verifySquadUniqueMembership();
	AKBot::PlayerLocationProvider& _locationProvider;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<UnitInfoManager> _unitInfo;
	shared_ptr<BaseLocationManager> _bases;
	shared_ptr<MapTools> _mapTools;
	const BotMicroConfiguration& _microConfiguration;
	const BotSparCraftConfiguration& _sparcraftConfiguration;
	const BotDebugConfiguration& _debugConfiguration;
public:

	SquadData(
		AKBot::PlayerLocationProvider& locationProvider,
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<MapTools> mapTools,
		std::shared_ptr<AKBot::Logger> logger,
		const BotMicroConfiguration& microConfiguration,
		const BotSparCraftConfiguration& sparcraftConfiguration,
		const BotDebugConfiguration& debugConfiguration);

    void            clearSquadData(int currentFrame);

    bool            canAssignUnitToSquad(BWAPI::Unit unit, const Squad & squad) const;
    void            assignUnitToSquad(BWAPI::Unit unit, Squad & squad);
    void            addSquad(const std::string & squadName, Squad & squad);
	void            addSquad(const std::string & squadName, const SquadOrder & squadOrder, size_t priority);
    void            removeSquad(const std::string & squadName, int currentFrame);

    void            update(shared_ptr<MapTools> map, int currentFrame);

    bool            squadExists(const std::string & squadName);
    bool            unitIsInSquad(BWAPI::Unit unit) const;
    const Squad *   getUnitSquad(BWAPI::Unit unit) const;
    Squad *         getUnitSquad(BWAPI::Unit unit);

    Squad &         getSquad(const std::string & squadName);
    const std::map<std::string, Squad> & getSquads() const;

	void onUnitRemoved(UnitHandler handler);
	void setRushModeEnabled(bool value);
	void setRushUnitType(BWAPI::UnitType rushUnit);
	void setAllowedRushUnitLoses(int value);
};
}