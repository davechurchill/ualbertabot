#pragma once

#include "Common.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include <memory>

namespace SparCraft
{
 
class GameState;

class Player 
{
protected:
    size_t _playerID;
public:
    virtual void		getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    const size_t        ID();
    void                setID(const size_t & playerid);
    virtual size_t      getType() { return PlayerModels::None; }
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

typedef	std::shared_ptr<Player> PlayerPtr; 

}