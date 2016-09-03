#pragma once

#include "Common.h"
#include "Action.h"
#include "Position.hpp"
#include "PlayerProperties.h"
#include "WeaponProperties.h"

namespace SparCraft
{

class Unit
{
    BWAPI::UnitType     _type;
    Position            _position;

    size_t              _unitID;
    size_t              _bwapiID;
    size_t              _playerID;

    size_t              _hp;
    size_t              _shields;
    size_t              _energy;
    size_t              _armor;

    size_t              _groundCD;
    size_t              _airCD;
    PlayerWeapon        _groundWeapon;
    PlayerWeapon        _airWeapon;

    double              _velocityX;
    double              _velocityY;

    Action              _action;
    size_t              _actionTime;

    void                takeDamage(const Unit & attacker);
    
public:

                        Unit();
                        Unit(const BWAPI::UnitType unitType,const size_t & playerID, const Position & pos);

    void                onFrame();
    void                setAction(const Action & action, const size_t & time);

    const size_t &      getID()         const;
    const size_t &      getPlayerID()   const;
    const size_t &      getGroundCD()   const;
    const size_t &      getAirCD()      const;
    const size_t &      getHP()         const;
    const size_t &      getShields()    const;
    const size_t &      getEnergy()     const;
    const size_t &      getArmor()      const;
    const double &      getVelocityX()  const;
    const double &      getVelocityY()  const;
    const Position &    getPosition()   const;
    const BWAPI::UnitType & getType()   const;

    const PlayerWeapon & getGroundWeapon() const;
    const PlayerWeapon & getAirWeapon() const;
    const PlayerWeapon & getWeapon(const Unit & target) const;

    bool                isAlive()       const;
};

}