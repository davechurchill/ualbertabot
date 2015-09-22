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

    bool            canAssignUnitToSquad(BWAPI::UnitInterface * unit, const Squad & squad) const;
    void            assignUnitToSquad(BWAPI::UnitInterface * unit, Squad & squad);
    void            addSquad(const std::string & squadName, const Squad & squad);
    void            removeSquad(const std::string & squadName);
    void            clearSquad(const std::string & squadName);
	void            drawSquadInformation(int x, int y);

    void            update();
    void            setSquad(const std::vector<BWAPI::UnitInterface *> & units, const SquadOrder & order);
    void            setRegroup();

    bool            squadExists(const std::string & squadName);
    bool            unitIsInSquad(BWAPI::UnitInterface * unit) const;
    const Squad *   getUnitSquad(BWAPI::UnitInterface * unit) const;
    Squad *         getUnitSquad(BWAPI::UnitInterface * unit);

    Squad &         getSquad(const std::string & squadName);
};
}