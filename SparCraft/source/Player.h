#pragma once

#include "Common.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"

#include <boost/shared_ptr.hpp>

namespace SparCraft
{
 
class GameState;

class Player 
{
protected:
    IDType _playerID;
public:
    virtual void		getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
    const IDType        ID();
    void                setID(const IDType & playerid);
    virtual IDType      getType() { return PlayerModels::None; }
};

class CompareUnitDPSThreat
{
    const bool operator() (Unit * u1, Unit * u2) const
    {
        double u1Threat = ((double)u1->damage()/(double)u1->attackCooldown()) / u1->currentHP();
        double u2Threat = ((double)u2->damage()/(double)u2->attackCooldown()) / u2->currentHP();

        return u1Threat > u2Threat;
    }
};

typedef	boost::shared_ptr<Player> PlayerPtr; 

}