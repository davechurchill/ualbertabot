#pragma once

#include "Common.h"

#include "Action.h"
#include "Position.hpp"
#include "Hash.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "AnimationFrameData.h"
#include <iostream>

namespace SparCraft
{

class Action;

class Unit 
{
    BWAPI::UnitType     _unitType;				// the BWAPI unit type that we are mimicing
    PositionType        _range;
	
	Position            _position;				// current location in a possibly infinite space
	
	IDType              _unitID;				// unique unit ID to the state it's contained in
    IDType              _playerID;				// the player who controls the unit
	
	HealthType          _currentHP;				// current HP of the unit
	HealthType          _currentEnergy;

	TimeType            _timeCanMove;			// time the unit can next move
	TimeType            _timeCanAttack;			// time the unit can next attack

	Action              _previousAction;		// the previous action that the unit performed
	TimeType            _previousActionTime;	// the time the previous move was performed
	Position            _previousPosition;

    mutable TimeType    _prevCurrentPosTime;
    mutable Position    _prevCurrentPos;

public:

	Unit();
	Unit(const BWAPI::UnitType unitType, const IDType & playerID, const Position & pos);
	Unit(const BWAPI::UnitType unitType, const Position & pos, const IDType & unitID, const IDType & playerID, 
		 const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta);

	const bool operator < (const Unit & rhs) const;

    // action functions
	void                    setPreviousAction(const Action & m, const TimeType & previousMoveTime);
	void                    updateAttackActionTime(const TimeType & newTime);
	void                    updateMoveActionTime(const TimeType & newTime);
	void                    attack(const Action & move, const Unit & target, const TimeType & gameTime);
	void                    heal(const Action & move, const Unit & target, const TimeType & gameTime);
	void                    move(const Action & move, const TimeType & gameTime) ;
	void                    waitUntilAttack(const Action & move, const TimeType & gameTime);
	void                    pass(const Action & move, const TimeType & gameTime);
	void                    takeAttack(const Unit & attacker);
	void                    takeHeal(const Unit & healer);

	// conditional functions
	const bool			    isMobile()                  const;
	const bool			    isOrganic()                 const;
	const bool              isAlive()                   const;
	const bool			    canAttackNow()              const;
	const bool			    canMoveNow()                const;
	const bool			    canHealNow()                const;
	const bool			    canKite()                   const;
	const bool			    canHeal()                   const;
	const bool              equalsID(const Unit & rhs)  const;
	bool					canSeeTarget(const Unit & unit, const TimeType & gameTime) const;
	const bool              canAttackTarget(const Unit & unit, const TimeType & gameTime) const;
	const bool              canHealTarget(const Unit & unit, const TimeType & gameTime) const;

    // id related
	void                    setUnitID(const IDType & id);
	const IDType		    ID()                        const;
	const IDType		    player()                    const;

    // position related functions
	const Position &        position()                  const;
	const Position &        pos()                       const;
	const PositionType      x()                         const;
	const PositionType      y()                         const;
	const PositionType      range()                     const;
	const PositionType      healRange()                 const;
	const PositionType      getDistanceSqToUnit(const Unit & u, const TimeType & gameTime) const;
	const PositionType      getDistanceSqToPosition(const Position & p, const TimeType & gameTime) const;
    const Position &        currentPosition(const TimeType & gameTime) const;
    void                    setPreviousPosition(const TimeType & gameTime);

    // health and damage related functions
	const HealthType        damage()                    const;
	const HealthType        healAmount()                const;
	const HealthType	    maxHP()                     const;
	const HealthType	    currentHP()                 const;
	const HealthType	    currentEnergy()             const;
	const HealthType	    maxEnergy()                 const;
	const HealthType	    healCost()                  const;
    const HealthType        getArmor()                  const;
	const float			    dpf()                       const;
	void                    updateCurrentHP(const HealthType & newHP);
    const BWAPI::UnitSizeType getSize()                 const;
    const BWAPI::WeaponType getWeapon(BWAPI::UnitType target) const;
    const HealthType        getDamageTo(const Unit & unit) const;
    const PlayerWeapon      getWeapon(const Unit & target) const;

    // time and cooldown related functions
	const TimeType		    moveCooldown()              const;
	const TimeType		    attackCooldown()            const;
	const TimeType		    healCooldown()              const;
	const TimeType		    nextAttackActionTime()      const;
	const TimeType		    nextMoveActionTime()        const;
	const TimeType		    previousActionTime()        const;
	const TimeType		    firstTimeFree()             const;
	const TimeType 		    attackInitFrameTime()       const;
	const TimeType 		    attackRepeatFrameTime()     const;
	void                    setCooldown(TimeType attack, TimeType move);

    // other functions
	const int			    typeID()                    const;
	const double		    speed()                     const;
	const BWAPI::UnitType   type()                      const;
	const Action &	        previousAction()            const;
	const std::string       name()                      const;
	void                    print()                     const;
    const std::string       debugString()               const;

	// hash functions
	const HashType          calculateHash(const size_t & hashNum, const TimeType & gameTime) const;
	void                    debugHash(const size_t & hashNum, const TimeType & gameTime) const;
};

class UnitPtrCompare
{
public:
	const bool operator() (Unit * u1, Unit * u2) const
	{
		return *u1 < *u2;
	}
};
}