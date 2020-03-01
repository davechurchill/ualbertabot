#pragma once

#include "Common.h"
#include "Squad.h"

namespace UAlbertaBot
{

class SquadData
{
    std::map<std::string, Squad> m_squads;

    void updateAllSquads();
    void verifySquadUniqueMembership();

public:

    SquadData();

    void update();
    void clearSquadData();
    void assignUnitToSquad(BWAPI::Unit unit, Squad & squad);
    void addSquad(const std::string & squadName, const Squad & squad);
    void removeSquad(const std::string & squadName);
    void drawSquadInformation(int x, int y);

    bool canAssignUnitToSquad(BWAPI::Unit unit, const Squad & squad) const;
    bool squadExists(const std::string & squadName);
    bool unitIsInSquad(BWAPI::Unit unit) const;
    const Squad * getUnitSquad(BWAPI::Unit unit) const;
    Squad * getUnitSquad(BWAPI::Unit unit);
    Squad & getSquad(const std::string & squadName);
    const std::map<std::string, Squad> & getSquads() const;
};
}