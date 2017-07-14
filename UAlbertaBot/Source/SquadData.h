#pragma once

#include "Squad.h"

namespace UAlbertaBot
{
class SquadData
{
	std::map<std::string, Squad> _squads;
	UnitHandler onRemoveHandler;

    void    updateAllSquads(const MapTools& map);
    void    verifySquadUniqueMembership();
	AKBot::PlayerLocationProvider& _locationProvider;

public:

	SquadData(AKBot::PlayerLocationProvider& locationProvider);

    void            clearSquadData();

    bool            canAssignUnitToSquad(BWAPI::Unit unit, const Squad & squad) const;
    void            assignUnitToSquad(BWAPI::Unit unit, Squad & squad);
    void            addSquad(const std::string & squadName, Squad & squad);
	void            addSquad(const std::string & squadName, const SquadOrder & squadOrder, size_t priority);
    void            removeSquad(const std::string & squadName);
	void            drawSquadInformation(AKBot::ScreenCanvas& canvas, int x, int y);

    void            update(const MapTools& map);

    bool            squadExists(const std::string & squadName);
    bool            unitIsInSquad(BWAPI::Unit unit) const;
    const Squad *   getUnitSquad(BWAPI::Unit unit) const;
    Squad *         getUnitSquad(BWAPI::Unit unit);

    Squad &         getSquad(const std::string & squadName);
    const std::map<std::string, Squad> & getSquads() const;

	void onUnitRemoved(UnitHandler handler);
};
}