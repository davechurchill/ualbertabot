#include "Unit.h"

using namespace SparCraft;

Unit::Unit()
    : _unitType             (BWAPI::UnitTypes::None)
    , _range                (0)
    , _unitID               (255)
    , _playerID             (255)
    , _currentHP            (0)
    , _currentEnergy        (0)
    , _timeCanMove          (0)
    , _timeCanAttack        (0)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
{
    
}

// test constructor for setting all variables of a unit
Unit::Unit(const BWAPI::UnitType unitType, const Position & pos, const IDType & unitID, const IDType & playerID, 
           const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta) 
    : _unitType             (unitType)
    , _range                (PlayerWeapon(&PlayerProperties::Get(playerID), unitType.groundWeapon()).GetMaxRange() + Constants::Range_Addition)
    , _position             (pos)
    , _unitID               (unitID)
    , _playerID             (playerID)
    , _currentHP            (hp)
    , _currentEnergy        (energy)
    , _timeCanMove          (tm)
    , _timeCanAttack        (ta)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
    , _previousPosition     (pos)
    , _prevCurrentPos       (pos)
{
    System::checkSupportedUnitType(unitType);
}

// constructor for units to construct basic units, sets some things automatically
Unit::Unit(const BWAPI::UnitType unitType, const IDType & playerID, const Position & pos) 
    : _unitType             (unitType)
    , _range                (PlayerWeapon(&PlayerProperties::Get(playerID), unitType.groundWeapon()).GetMaxRange() + Constants::Range_Addition)
    , _position             (pos)
    , _unitID               (0)
    , _playerID             (playerID)
    , _currentHP            (maxHP())
    , _currentEnergy        (unitType == BWAPI::UnitTypes::Terran_Medic ? Constants::Starting_Energy : 0)
    , _timeCanMove          (0)
    , _timeCanAttack        (0)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
    , _previousPosition     (pos)
    , _prevCurrentPos       (pos)
{
    System::checkSupportedUnitType(unitType);
}

// Less than operator, used for sorting the GameState unit array.
// Units are sorted in this order:
//		1) alive < dead
//		2) firstTimeFree()
//		3) currentHP()
//		4) pos()
const bool Unit::operator < (const Unit & rhs) const
{
    if (!isAlive())
    {
        return false;
    }
    else if (!rhs.isAlive())
    {
        return true;
    }

    if (firstTimeFree() == rhs.firstTimeFree())
    {
        return ID() < rhs.ID();
    }
    else
    {
        return firstTimeFree() < rhs.firstTimeFree();
    }

    /*if (firstTimeFree() == rhs.firstTimeFree())
    {
        if (currentHP() == rhs.currentHP())
        {
            return pos() < rhs.pos();
        }
        else
        {
            return currentHP() < rhs.currentHP();
        }
    }
    else
    {
        return firstTimeFree() < rhs.firstTimeFree();
    }*/
}

// compares a unit based on unit id
const bool Unit::equalsID(const Unit & rhs) const
{ 
    return _unitID == rhs._unitID; 
}
// returns whether or not this unit can see a given unit at a given time
bool Unit::canSeeTarget(const Unit & unit, const TimeType & gameTime) const
{

	// range of this unit attacking
	PositionType r = type().sightRange();

	// return whether the target unit is in range
	return (r * r) >= getDistanceSqToUnit(unit, gameTime);
}

// returns whether or not this unit can attack a given unit at a given time
const bool Unit::canAttackTarget(const Unit & unit, const TimeType & gameTime) const
{
    BWAPI::WeaponType weapon = unit.type().isFlyer() ? type().airWeapon() : type().groundWeapon();

    if (weapon.damageAmount() == 0)
    {
        return false;
    }

    // range of this unit attacking
    PositionType r = range();

    // return whether the target unit is in range
    return (r * r) >= getDistanceSqToUnit(unit, gameTime);
}

const bool Unit::canHealTarget(const Unit & unit, const TimeType & gameTime) const
{
    // if the unit can't heal or the target unit is not on the same team
    if (!canHeal() || !unit.isOrganic() || !(unit.player() == player()) || (unit.currentHP() == unit.maxHP()))
    {
        // then it can't heal the target
        return false;
    }

    // range of this unit attacking
    PositionType r = healRange();

    // return whether the target unit is in range
    return (r * r) >= getDistanceSqToUnit(unit, gameTime);
}

const Position & Unit::position() const
{
    return _position;
}

// take an attack, subtract the hp
void Unit::takeAttack(const Unit & attacker)
{
    PlayerWeapon    weapon(attacker.getWeapon(*this));
    HealthType      damage(weapon.GetDamageBase());

    // calculate the damage based on armor and damage types
    damage = std::max((int)((damage-getArmor()) * weapon.GetDamageMultiplier(getSize())), 2);
    
    // special case where units attack multiple times
    if (attacker.type() == BWAPI::UnitTypes::Protoss_Zealot || attacker.type() == BWAPI::UnitTypes::Terran_Firebat)
    {
        damage *= 2;
    }

    //std::cout << type().getName() << " took " << (int)attacker.player() << " " << damage << "\n";

    updateCurrentHP(_currentHP - damage);
}

void Unit::takeHeal(const Unit & healer)
{
    updateCurrentHP(_currentHP + healer.healAmount());
}

// returns whether or not this unit is alive
const bool Unit::isAlive() const
{
    return _currentHP > 0;
}

// attack a unit, set the times accordingly
void Unit::attack(const Action & move, const Unit & target, const TimeType & gameTime)
{
    // if this is a repeat attack
    if (_previousAction.type() == ActionTypes::ATTACK || _previousAction.type() == ActionTypes::RELOAD)
    {
        // add the repeat attack animation duration
        // can't attack again until attack cooldown is up
        updateMoveActionTime      (gameTime + attackRepeatFrameTime());
        updateAttackActionTime    (gameTime + attackCooldown());
    }
    // if there previous action was a MOVE action, add the move penalty
    else if (_previousAction.type() == ActionTypes::MOVE)
    {
        updateMoveActionTime      (gameTime + attackInitFrameTime() + 2);
        updateAttackActionTime    (gameTime + attackCooldown() + Constants::Move_Penalty);
    }
    else
    {
        // add the initial attack animation duration
        updateMoveActionTime      (gameTime + attackInitFrameTime() + 2);
        updateAttackActionTime    (gameTime + attackCooldown());
    }

    // if the unit is not mobile, set its next move time to its next attack time
    if (!isMobile())
    {
        updateMoveActionTime(_timeCanAttack);
    }

    setPreviousAction(move, gameTime);
}

// attack a unit, set the times accordingly
void Unit::heal(const Action & move, const Unit & target, const TimeType & gameTime)
{
    _currentEnergy -= healCost();

    // can't attack again until attack cooldown is up
    updateAttackActionTime        (gameTime + healCooldown());
    updateMoveActionTime          (gameTime + healCooldown());

    if (currentEnergy() < healCost())
    {
        updateAttackActionTime(1000000);
    }

    setPreviousAction(move, gameTime);
}

// unit update for moving based on a given Move
void Unit::move(const Action & move, const TimeType & gameTime) 
{
    _previousPosition = pos();

    // get the distance to the move action destination
    PositionType dist = move.pos().getDistance(pos());
    
    // how long will this move take?
    TimeType moveDuration = (TimeType)((double)dist / speed());

    // update the next time we can move, make sure a move always takes 1 time step
    updateMoveActionTime(gameTime + std::max(moveDuration, 1));

    // assume we need 4 frames to turn around after moving
    updateAttackActionTime(std::max(nextAttackActionTime(), nextMoveActionTime()));

    // update the position
    //_position.addPosition(dist * dir.x(), dist * dir.y());
    _position.moveTo(move.pos());

    setPreviousAction(move, gameTime);
}

// unit is commanded to wait until his attack cooldown is up
void Unit::waitUntilAttack(const Action & move, const TimeType & gameTime)
{
    // do nothing until we can attack again
    updateMoveActionTime(_timeCanAttack);
    setPreviousAction(move, gameTime);
}

void Unit::pass(const Action & move, const TimeType & gameTime)
{
    updateMoveActionTime(gameTime + Constants::Pass_Move_Duration);
    updateAttackActionTime(gameTime + Constants::Pass_Move_Duration);
    setPreviousAction(move, gameTime);
}

const PositionType Unit::getDistanceSqToUnit(const Unit & u, const TimeType & gameTime) const 
{ 
    return getDistanceSqToPosition(u.currentPosition(gameTime), gameTime); 
}

const PositionType Unit::getDistanceSqToPosition(const Position & p, const TimeType & gameTime) const	
{ 
    return currentPosition(gameTime).getDistanceSq(p);
}

// returns current position based on game time
const Position & Unit::currentPosition(const TimeType & gameTime) const
{
    // if the previous move was MOVE, then we need to calculate where the unit is now
    if (_previousAction.type() == ActionTypes::MOVE)
    {
        // if gameTime is equal to previous move time then we haven't moved yet
        if (gameTime == _previousActionTime)
        {
            return _previousPosition;
        }
        // else if game time is >= time we can move, then we have arrived at the destination
        else if (gameTime >= _timeCanMove)
        {
            return _position;
        }
        // otherwise we are still moving, so calculate the current position
        else if (gameTime == _prevCurrentPosTime)
        {
            return _prevCurrentPos;
        }
        else
        {
            TimeType moveDuration = _timeCanMove - _previousActionTime;
            float moveTimeRatio = (float)(gameTime - _previousActionTime) / moveDuration;
            _prevCurrentPosTime = gameTime;

            // calculate the new current position
            _prevCurrentPos = _position;
            _prevCurrentPos.subtractPosition(_previousPosition);
            _prevCurrentPos.scalePosition(moveTimeRatio);
            _prevCurrentPos.addPosition(_previousPosition);

            //_prevCurrentPos = _previousPosition + (_position - _previousPosition).scale(moveTimeRatio);
            return _prevCurrentPos;
        }
    }
    // if it wasn't a MOVE, then we just return the Unit position
    else
    {
        return _position;
    }
}

void Unit::setPreviousPosition(const TimeType & gameTime)
{
    TimeType moveDuration = _timeCanMove - _previousActionTime;
    float moveTimeRatio = (float)(gameTime - _previousActionTime) / moveDuration;
    _prevCurrentPosTime = gameTime;
    _prevCurrentPos = _previousPosition + (_position - _previousPosition).scale(moveTimeRatio);
}

// returns the damage a unit does
const HealthType Unit::damage() const	
{ 
    return _unitType == BWAPI::UnitTypes::Protoss_Zealot ? 
        2 * (HealthType)_unitType.groundWeapon().damageAmount() : 
    (HealthType)_unitType.groundWeapon().damageAmount(); 
}

const HealthType Unit::healAmount() const
{
    return canHeal() ? 6 : 0;
}

void Unit::print() const 
{ 
    printf("%s %5d [%5d %5d] (%5d, %5d)\n", _unitType.getName().c_str(), currentHP(), nextAttackActionTime(), nextMoveActionTime(), x(), y()); 
}

void Unit::updateCurrentHP(const HealthType & newHP) 
{ 
    _currentHP = std::min(maxHP(), newHP); 
}

void Unit::updateAttackActionTime(const TimeType & newTime)
{ 
    _timeCanAttack = newTime; 
}

void Unit::updateMoveActionTime(const TimeType & newTime)
{ 
    _timeCanMove = newTime; 
} 

void Unit::setCooldown(TimeType attack, TimeType move)
{ 
    _timeCanAttack = attack; _timeCanMove = move; 
}

void Unit::setUnitID(const IDType & id)
{ 
    _unitID = id; 
}

void Unit::setPreviousAction(const Action & m, const TimeType & previousMoveTime) 
{	
    // if it was an attack move, store the unitID of the opponent unit
    _previousAction = m;
    _previousActionTime = previousMoveTime; 
}

const bool Unit::canAttackNow() const
{ 
    return !canHeal() && _timeCanAttack <= _timeCanMove; 
}

const bool Unit::canMoveNow() const
{ 
    return isMobile() && _timeCanMove <= _timeCanAttack; 
}

const bool Unit::canHealNow() const
{ 
    return canHeal() && (currentEnergy() >= healCost()) && (_timeCanAttack <= _timeCanMove); 
}

const bool Unit::canKite() const
{ 
    return _timeCanMove < _timeCanAttack; 
}

const bool Unit::isMobile() const
{ 
    return _unitType.canMove(); 
}

const bool Unit::canHeal() const
{ 
    return _unitType == BWAPI::UnitTypes::Terran_Medic; 
}

const bool Unit::isOrganic() const
{ 
    return _unitType.isOrganic(); 
}

const IDType Unit::ID() const	
{ 
    return _unitID; 
}

const IDType Unit::player() const
{ 
    return _playerID; 
}

const Position & Unit::pos() const
{ 
    return _position; 
}

const PositionType Unit::x() const 
{ 
    return _position.x(); 
}

const PositionType Unit::y() const 
{ 
    return _position.y(); 
}

const PositionType Unit::range() const 
{ 
    return _range; 
}

const PositionType Unit::healRange() const
{ 
    return canHeal() ? 96 : 0; 
}

const HealthType Unit::maxHP() const 
{ 
    return (HealthType)_unitType.maxHitPoints() + (HealthType)_unitType.maxShields(); 
}

const HealthType Unit::currentHP() const 
{ 
    return (HealthType)_currentHP; 
}

const HealthType Unit::currentEnergy() const 
{ 
    return (HealthType)_currentEnergy; 
}

const HealthType Unit::maxEnergy() const
{ 
    return (HealthType)_unitType.maxEnergy(); 
}

const HealthType Unit::healCost() const	
{ 
    return 3; 
}

const float Unit::dpf() const 
{ 
    return (float)std::max(Constants::Min_Unit_DPF, (float)damage() / ((float)attackCooldown() + 1)); 
}

const TimeType Unit::moveCooldown() const 
{ 
    return (TimeType)((double)Constants::Move_Distance / _unitType.topSpeed()); 
}

const TimeType Unit::attackCooldown() const 
{ 
    return (TimeType)_unitType.groundWeapon().damageCooldown(); 
}

const TimeType Unit::healCooldown() const 
{ 
    return (TimeType)8; 
}

const TimeType Unit::nextAttackActionTime() const 
{ 
    return _timeCanAttack; 
}

const TimeType Unit::nextMoveActionTime() const	
{ 
    return _timeCanMove; 
}

const TimeType Unit::previousActionTime() const	
{ 
    return _previousActionTime; 
}

const TimeType Unit::firstTimeFree() const	
{ 
    return _timeCanAttack <= _timeCanMove ? _timeCanAttack : _timeCanMove; 
}

const TimeType Unit::attackInitFrameTime() const	
{ 
    return AnimationFrameData::getAttackFrames(_unitType).first; 
}

const TimeType Unit::attackRepeatFrameTime() const	
{
    return AnimationFrameData::getAttackFrames(_unitType).second; 
}

const int Unit::typeID() const	
{ 
    return _unitType.getID(); 
}

const double Unit::speed() const 
{ 
    return _unitType.topSpeed(); 
}

const BWAPI::UnitType Unit::type() const 
{ 
    return _unitType; 
}

const Action & Unit::previousAction() const 
{ 
    return _previousAction; 
}

const BWAPI::UnitSizeType Unit::getSize() const
{
    return _unitType.size();
}

const PlayerWeapon Unit::getWeapon(const Unit & target) const
{
    return PlayerWeapon(&PlayerProperties::Get(player()), target.type().isFlyer() ? _unitType.airWeapon() : _unitType.groundWeapon());
}

const HealthType Unit::getArmor() const
{
    return UnitProperties::Get(type()).GetArmor(PlayerProperties::Get(player())); 
}

const BWAPI::WeaponType Unit::getWeapon(BWAPI::UnitType target) const
{
    return target.isFlyer() ? _unitType.airWeapon() : _unitType.groundWeapon();
}

const std::string Unit::name() const 
{ 
    std::string n(_unitType.getName());
    std::replace(n.begin(), n.end(), ' ', '_');
    return n;
}

// calculates the hash of this unit based on a given game time
const HashType Unit::calculateHash(const size_t & hashNum, const TimeType & gameTime) const
{
    Position currentPos = currentPosition(gameTime);

    return	  Hash::values[hashNum].positionHash(_playerID, currentPos.x(), currentPos.y()) 
            ^ Hash::values[hashNum].getAttackHash(_playerID, nextAttackActionTime() - gameTime) 
            ^ Hash::values[hashNum].getMoveHash(_playerID, nextMoveActionTime() - gameTime)
            ^ Hash::values[hashNum].getCurrentHPHash(_playerID, currentHP())
            ^ Hash::values[hashNum].getUnitTypeHash(_playerID, typeID());
}

// calculates the hash of this unit based on a given game time, and prints debug info
void Unit::debugHash(const size_t & hashNum, const TimeType & gameTime) const
{
    std::cout << " Pos   " << Hash::values[hashNum].positionHash(_playerID, position().x(), position().y());
    std::cout << " Att   " << Hash::values[hashNum].getAttackHash(_playerID, nextAttackActionTime() - gameTime);
    std::cout << " Mov   " << Hash::values[hashNum].getMoveHash(_playerID, nextMoveActionTime() - gameTime);
    std::cout << " HP    " << Hash::values[hashNum].getCurrentHPHash(_playerID, currentHP());
    std::cout << " Typ   " << Hash::values[hashNum].getUnitTypeHash(_playerID, typeID()) << "\n";;

    HashType hash = Hash::values[hashNum].positionHash(_playerID, position().x(), position().y()); std::cout << hash << "\n";
    hash ^= Hash::values[hashNum].getAttackHash(_playerID, nextAttackActionTime() - gameTime) ; std::cout << hash << "\n";
    hash ^= Hash::values[hashNum].getMoveHash(_playerID, nextMoveActionTime() - gameTime); std::cout << hash << "\n";
    hash ^= Hash::values[hashNum].getCurrentHPHash(_playerID, currentHP()); std::cout << hash << "\n";
    hash ^= Hash::values[hashNum].getUnitTypeHash(_playerID, typeID()); std::cout << hash << "\n";
}

const std::string Unit::debugString() const
{
    std::stringstream ss;

    ss << "Unit Type:           " << type().getName()                               << "\n";
    ss << "Unit ID:             " << (int)ID()                                      << "\n";
    ss << "Player:              " << (int)player()                                  << "\n";
    ss << "Range:               " << range()                                        << "\n";
    ss << "Position:            " << "(" << _position.x() << "," << _position.y()   << ")\n";
    ss << "Current HP:          " << currentHP()                                    << "\n";
    ss << "Next Move Time:      " << nextMoveActionTime()                           << "\n";
    ss << "Next Attack Time:    " << nextAttackActionTime()                         << "\n";
    ss << "Previous Action:     " << previousAction().debugString()                 << "\n";
    ss << "Previous Pos Time:   " << _prevCurrentPosTime                            << "\n";
    ss << "Previous Pos:        " << "(" << _prevCurrentPos.x() << "," << _prevCurrentPos.y()   << ")\n";

    return ss.str();
}