#include "Unit.h"

using namespace SparCraft;

Unit::Unit()
    : _unitType             (BWAPI::UnitTypes::None)
    , _range                (0)
    , _unitID               (0)
    , _bwapiID              (0)
    , _playerID             (0)
    , _currentHP            (0)
    , _timeCanMove          (0)
    , _timeCanAttack        (0)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
    , _canAttackGround      (false)
    , _canAttackAir         (false)
    , _isFlyer              (false)
{
    
}

// test constructor for setting all variables of a unit
Unit::Unit(const BWAPI::UnitType unitType, const Position & pos, const size_t & unitID, const size_t & playerID, 
           const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta) 
    : _unitType             (unitType)
    , _range                (PlayerWeapon(&PlayerProperties::Get(playerID), unitType.groundWeapon()).GetMaxRange() + Config::Units::UnitRangeAddition)
    , _position             (pos)
    , _unitID               (unitID)
    , _bwapiID              (0)
    , _playerID             (playerID)
    , _currentHP            (hp)
    , _timeCanMove          (tm)
    , _timeCanAttack        (ta)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
    , _previousPosition     (pos)
    , _prevCurrentPos       (pos)
{
    _canAttackAir = unitType.airWeapon().damageAmount() > 0;
    _canAttackGround = unitType.groundWeapon().damageAmount() > 0;
    _isFlyer = unitType.isFlyer();

    SPARCRAFT_ASSERT(System::UnitTypeSupported(unitType), "Unit type not supported: %s", unitType.getName().c_str());
}

// constructor for units to construct basic units, sets some things automatically
Unit::Unit(const BWAPI::UnitType unitType, const size_t & playerID, const Position & pos) 
    : _unitType             (unitType)
    , _range                (PlayerWeapon(&PlayerProperties::Get(playerID), unitType.groundWeapon()).GetMaxRange() + Config::Units::UnitRangeAddition)
    , _position             (pos)
    , _unitID               (0)
    , _bwapiID              (0)
    , _playerID             (playerID)
    , _currentHP            (maxHP())
    , _timeCanMove          (0)
    , _timeCanAttack        (0)
    , _previousActionTime   (0)
    , _prevCurrentPosTime   (0)
    , _previousPosition     (pos)
    , _prevCurrentPos       (pos)
{
    _canAttackAir = unitType.airWeapon().damageAmount() > 0;
    _canAttackGround = unitType.groundWeapon().damageAmount() > 0;
    _isFlyer = unitType.isFlyer();

    SPARCRAFT_ASSERT(System::UnitTypeSupported(unitType), "Unit type not supported: %s", unitType.getName().c_str());
}

const bool Unit::canAttackAir() const
{
    return _canAttackAir;
}

const bool Unit::canAttackGround() const
{
    return _canAttackGround;
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
	int r = type().sightRange();

	// return whether the target unit is in range
	return (r * r) >= getDistanceSqToUnit(unit, gameTime);
}

const bool Unit::canTarget(const Unit & unit) const
{
    if (unit.isFlyer() && !canAttackAir())
    {
        return false;
    }
    else if (!unit.isFlyer() && !canAttackGround())
    {
        return false;
    }

    return true;
}

// returns whether or not this unit can attack a given unit at a given time
const bool Unit::canAttackTarget(const Unit & unit, const TimeType & gameTime) const
{
    if (!canTarget(unit))
    {
        return false;
    }

    // range of this unit attacking
    int r = range();

    // return whether the target unit is in range
    return (r * r) >= getDistanceSqToUnit(unit, gameTime);
}

const Position & Unit::position() const
{
    return _position;
}

const HealthType Unit::damageTakenFrom(const Unit & attacker) const
{
    const PlayerWeapon & weapon = attacker.getWeapon(*this);
    HealthType damage = weapon.GetDamageBase();

    // calculate the damage based on armor and damage types
    damage = std::max((int)((damage-getArmor()) * weapon.GetDamageMultiplier(getSize())), 2);
    
    // special case where units attack multiple times
    if (attacker.type() == BWAPI::UnitTypes::Protoss_Zealot || attacker.type() == BWAPI::UnitTypes::Terran_Firebat)
    {
        damage *= 2;
    }

    return damage;
}

// take an attack, subtract the hp
void Unit::takeAttack(const Unit & attacker)
{
    updateCurrentHP(_currentHP - damageTakenFrom(attacker));
}

// returns whether or not this unit is alive
const bool Unit::isAlive() const
{
    return _currentHP > 0;
}

// attack a unit, set the times accordingly
void Unit::attack(const Action & move, const Unit & target, const TimeType & gameTime)
{
    SPARCRAFT_ASSERT(nextAttackActionTime() == gameTime, "Trying to attack when we can't");

    // if this is a repeat attack
    if (_previousAction.type() == ActionTypes::ATTACK || _previousAction.type() == ActionTypes::RELOAD)
    {
        // add the repeat attack animation duration
        // can't attack again until attack cooldown is up
        updateMoveActionTime      (gameTime + attackRepeatFrameTime());
        updateAttackActionTime    (gameTime + attackCooldown());
    }
    // if there previous action was a MOVE action
    else if (_previousAction.type() == ActionTypes::MOVE)
    {
        updateMoveActionTime      (gameTime + attackInitFrameTime() + Config::Units::UnitMoveAfterAttackBuffer);
        updateAttackActionTime    (gameTime + attackCooldown());
    }
    else
    {
        // add the initial attack animation duration
        updateMoveActionTime      (gameTime + attackInitFrameTime());
        updateAttackActionTime    (gameTime + attackCooldown());
    }

    // if the unit is not mobile, set its next move time to its next attack time
    if (!isMobile())
    {
        updateMoveActionTime(_timeCanAttack);
    }

    setPreviousAction(move, gameTime);
}

// unit update for moving based on a given Move
void Unit::move(const Action & action, const TimeType & gameTime) 
{
    SPARCRAFT_ASSERT(nextMoveActionTime() == gameTime, "Trying to move when we can't");

    _previousPosition = pos();

    // get the distance to the move action destination
    double dist = action.pos().getDistance(pos());
    
    // how long will this move take?
    TimeType moveDuration = (TimeType)((double)dist / speed());

    // update the next time we can move, make sure a move always takes 1 time step
    updateMoveActionTime(gameTime + std::max(moveDuration, 1));

    const int movePenalty = isFlyer() ? Config::Units::AirUnitMovePenalty : Config::Units::GroundUnitMovePenalty;

    // assume we need 4 frames to turn around after moving
    updateAttackActionTime(movePenalty + std::max(nextAttackActionTime(), nextMoveActionTime()));

    // update the position
    //_position.addPosition(dist * dir.x(), dist * dir.y());
    _position.moveTo(action.pos());

    setPreviousAction(action, gameTime);

    SPARCRAFT_ASSERT(_previousActionTime < nextMoveActionTime(), "Move didn't take any time");
}

// unit is commanded to wait until his attack cooldown is up
void Unit::reload(const Action & action, const TimeType & gameTime)
{
    SPARCRAFT_ASSERT(_timeCanAttack > gameTime, "Attempting to reload when we can attack this frame\n\n%s", debugString().c_str());

    // do nothing until we can attack again
    updateMoveActionTime(_timeCanAttack);
    setPreviousAction(action, gameTime);
}

void Unit::pass(const Action & action, const TimeType & gameTime)
{
    updateMoveActionTime(gameTime + action.getTargetID());
    updateAttackActionTime(gameTime + action.getTargetID());
    setPreviousAction(action, gameTime);
}

const int Unit::getDistanceSqToUnit(const Unit & u, const TimeType & gameTime) const 
{ 
    return getDistanceSqToPosition(u.currentPosition(gameTime), gameTime); 
}

const int Unit::getDistanceSqToPosition(const Position & p, const TimeType & gameTime) const	
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
    HealthType damage = _unitType == BWAPI::UnitTypes::Protoss_Zealot ? (2 * (HealthType)_unitType.groundWeapon().damageAmount()) : (HealthType)_unitType.groundWeapon().damageAmount(); 
    
    damage = std::max(damage, (HealthType)_unitType.airWeapon().damageAmount());
    
    return damage;
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

void Unit::setUnitID(const size_t & id)
{ 
    _unitID = id; 
}

void Unit::setBWAPIUnitID(const size_t & id)
{ 
    _bwapiID = id; 
}

void Unit::setPreviousAction(const Action & m, const TimeType & previousMoveTime) 
{	
    // if it was an attack move, store the unitID of the opponent unit
    _previousAction = m;
    _previousActionTime = previousMoveTime; 
}

const bool Unit::canAttackNow() const
{ 
    return _timeCanAttack <= _timeCanMove; 
}

const bool Unit::canMoveNow() const
{ 
    return isMobile() && _timeCanMove <= _timeCanAttack; 
}

const bool Unit::canKite() const
{ 
    return _timeCanMove < _timeCanAttack; 
}

const bool Unit::isMobile() const
{ 
    return _unitType.canMove(); 
}

const bool Unit::isOrganic() const
{ 
    return _unitType.isOrganic(); 
}

const size_t Unit::getID() const	
{ 
    return _unitID; 
}

const size_t Unit::getPlayerID() const
{ 
    return _playerID; 
}

const Position & Unit::pos() const
{ 
    return _position; 
}

const int Unit::x() const 
{ 
    return _position.x(); 
}

const int Unit::y() const 
{ 
    return _position.y(); 
}

const int Unit::range() const 
{ 
    return _range; 
}

const HealthType Unit::maxHP() const 
{ 
    return (HealthType)_unitType.maxHitPoints() + (HealthType)_unitType.maxShields(); 
}

const HealthType Unit::currentHP() const 
{ 
    return (HealthType)_currentHP; 
}

const HealthType Unit::maxEnergy() const
{ 
    return (HealthType)_unitType.maxEnergy(); 
}

const float Unit::dpf() const 
{ 
    if (damage() == 0 || attackCooldown() == 0)
    {
        return 0;
    }

    return (float)damage() / attackCooldown(); 
}

const TimeType Unit::attackCooldown() const 
{ 
    TimeType attackCooldown = _unitType.groundWeapon().damageCooldown(); 
    if (attackCooldown == 0) attackCooldown = _unitType.airWeapon().damageCooldown();
    return attackCooldown;
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
    return Config::Units::GetAttackFrames(_unitType).first; 
}

const TimeType Unit::attackRepeatFrameTime() const	
{
    return Config::Units::GetAttackFrames(_unitType).second; 
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

const bool Unit::isFlyer() const
{
    return _isFlyer;
}

const PlayerWeapon Unit::getWeapon(const Unit & target) const
{
    return PlayerWeapon(&PlayerProperties::Get(getPlayerID()), target.isFlyer() ? _unitType.airWeapon() : _unitType.groundWeapon());
}

const HealthType Unit::getArmor() const
{
    return UnitProperties::Get(type()).GetArmor(PlayerProperties::Get(getPlayerID())); 
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

const size_t Unit::getBWAPIUnitID() const
{
    return _bwapiID;
}

const std::string Unit::debugString() const
{
    std::stringstream ss;

    ss << "Unit Type:           " << type().getName()                               << "\n";
    ss << "Unit ID:             " << (int)getID()                               << "\n";
    ss << "Player:              " << (int)getPlayerID()                             << "\n";
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