#include "Unit.h"
#include "UnitProperties.h"

using namespace SparCraft;

Unit::Unit()
    : _unitID       (0)
    , _bwapiID      (0)
    , _playerID     (0)
    , _hp           (0)
    , _shields      (0)
    , _energy       (0)
    , _groundCD     (0)
    , _airCD        (0)
    , _groundWeapon (PlayerWeapon(PlayerProperties::Get(0), BWAPI::WeaponTypes::None))
    , _airWeapon    (PlayerWeapon(PlayerProperties::Get(0), BWAPI::WeaponTypes::None))
    , _velocityX    (0)
    , _velocityY    (0)
{

}

Unit::Unit(const BWAPI::UnitType unitType,const size_t & playerID, const Position & pos)
    : Unit()
{
    _type           = unitType;
    _playerID       = playerID;
    _position       = pos;
    _groundWeapon   = PlayerWeapon(PlayerProperties::Get(0), unitType.groundWeapon());
    _airWeapon      = PlayerWeapon(PlayerProperties::Get(0), unitType.airWeapon());
    _armor          = UnitProperties::Get(getType()).GetArmor(PlayerProperties::Get(getPlayerID()));
    _velocityX      = unitType.topSpeed();
    _velocityY      = unitType.topSpeed();
}

void Unit::takeDamage(const Unit & attacker)
{
    PlayerWeapon    weapon = attacker.getWeapon(*this);
    int             damage = weapon.GetDamageBase();

    // calculate the damage based on armor and damage types
    damage = std::max((int)((damage-getArmor()) * weapon.GetDamageMultiplier(_type.size())), 2);
    
    // special case where units attack multiple times
    if (attacker.getType() == BWAPI::UnitTypes::Protoss_Zealot || attacker.getType() == BWAPI::UnitTypes::Terran_Firebat)
    {
        damage *= 2;
    }

    //std::cout << type().getName() << " took " << (int)attacker.getPlayerID() << " " << damage << "\n";
    _hp = std::max(0, (int)_hp - damage);
}

const size_t & Unit::getArmor() const
{
    return _armor;
}

const PlayerWeapon & Unit::getGroundWeapon() const
{
    return _groundWeapon;
}

const PlayerWeapon & Unit::getAirWeapon() const
{
    return _airWeapon;
}

const PlayerWeapon & Unit::getWeapon(const Unit & target) const
{
    return target.getType().isFlyer() ? getAirWeapon() : getGroundWeapon();
}

void Unit::onFrame()
{
    // do stuff
}

bool Unit::isAlive() const
{
    return _hp > 0;
}

void Unit::setAction(const Action & action, const size_t & time)
{
    _action = action;
    _actionTime = time;
}

const size_t & Unit::getID() const
{
    return _unitID;
}

const size_t & Unit::getGroundCD() const
{
    return _groundCD;
}

const size_t & Unit::getAirCD() const
{
    return _airCD;
}

const double & Unit::getVelocityX() const
{
    return _velocityX;
}

const double & Unit::getVelocityY() const
{
    return _velocityY;
}

const size_t & Unit::getHP() const
{
    return _hp;
}

const size_t & Unit::getShields() const
{
    return _shields;
}

const size_t & Unit::getEnergy() const
{
    return _energy;
}

const Position & Unit::getPosition() const
{
    return _position;
}

const BWAPI::UnitType & Unit::getType() const
{
    return _type;
}

        
    