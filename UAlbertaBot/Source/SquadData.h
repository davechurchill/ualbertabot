#pragma once

#include "Squad.h"

namespace UAlbertaBot
{
class SquadData
{
	std::map<std::string, Squad> _squads;

    void    updateAllSquads();
    void    verifySquadUniqueMembership();

public:

	SquadData();

    void            clearSquadData();

    bool            canAssignUnitToSquad(BWAPI::Unit unit, const Squad & squad) const;
    void            assignUnitToSquad(BWAPI::Unit unit, Squad & squad);
    void            addSquad(const std::string & squadName, const Squad & squad);
    void            removeSquad(const std::string & squadName);
    void            clearSquad(const std::string & squadName);
	void            drawSquadInformation(int x, int y);

    void            update();
    void            setRegroup();

    bool            squadExists(const std::string & squadName);
    bool            unitIsInSquad(BWAPI::Unit unit) const;
    const Squad *   getUnitSquad(BWAPI::Unit unit) const;
    Squad *         getUnitSquad(BWAPI::Unit unit);

    Squad &         getSquad(const std::string & squadName);
    const std::map<std::string, Squad> & getSquads() const;
};
}