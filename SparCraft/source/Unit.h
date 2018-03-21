#pragma once

#include "Common.h"

#include "Action.h"
#include "Position.hpp"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include <iostream>

namespace SparCraft
{

class Unit 
{
    BWAPI::UnitType     _unitType;				// the BWAPI unit type that we are mimicing
    int                 _range;
	
	Position            _position;				// current location in a possibly infinite space
	
	size_t              _unitID;				// unique unit ID to the state it's contained in
    size_t              _bwapiID;
    size_t              _playerID;				// the player who controls the unit
	
	HealthType          _currentHP;				// current HP of the unit

	TimeType            _timeCanMove;			// time the unit can next move
	TimeType            _timeCanAttack;			// time the unit can next attack

	Action              _previousAction;		// the previous action that the unit performed
	TimeType            _previousActionTime;	// the time the previous move was performed
	Position            _previousPosition;

    bool                _isFlyer;
    bool                _canAttackAir;
    bool                _canAttackGround;

    mutable TimeType    _prevCurrentPosTime;
    mutable Position    _prevCurrentPos;
	TimeType			_removeTimer;

public:

	Unit();
	Unit(const BWAPI::UnitType unitType, const size_t & playerID, const Position & pos);
	Unit(const BWAPI::UnitType unitType, const Position & pos, const size_t & unitID, const size_t & playerID, 
		 const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta);

    // action functions
	void                    setPreviousAction(const Action & m, const TimeType & previousMoveTime);
	void                    updateAttackActionTime(const TimeType & newTime);
	void                    updateMoveActionTime(const TimeType & newTime);
	void                    attack(const Action & move, const Unit & target, const TimeType & gameTime);
	void                    move(const Action & move, const TimeType & gameTime) ;
	void                    reload(const Action & move, const TimeType & gameTime);
	void                    pass(const Action & move, const TimeType & gameTime);
	void                    takeAttack(const Unit & attacker);
	void setRemoveTimer(const TimeType removeTimer) { _removeTimer = removeTimer; }
	const TimeType& getRemoveTimer() const { return _removeTimer; }

	// conditional functions
	const bool			    isMobile()                  const;
	const bool			    isOrganic()                 const;
	const bool              isAlive()                   const;
	const bool			    canAttackNow()              const;
    const bool			    canAttackAir()              const;
    const bool			    canAttackGround()           const;
	const bool			    canMoveNow()                const;
	const bool			    canKite()                   const;
    const bool			    isFlyer()                   const;
	const bool              equalsID(const Unit & rhs)  const;
	bool					canSeeTarget(const Unit & unit, const TimeType & gameTime) const;
	const bool              canAttackTarget(const Unit & unit, const TimeType & gameTime) const;
    const bool              canTarget(const Unit & unit) const;

    // id related
	void                    setUnitID(const size_t & id);
    void                    setBWAPIUnitID(const size_t & id);
	const size_t            getID()                     const;
	const size_t		    getPlayerID()               const;
    const size_t            getBWAPIUnitID()            const;

    // position related functions
	const Position &        position()                  const;
	const Position &        pos()                       const;
	const int      x()                         const;
	const int      y()                         const;
	const int      range()                     const;
	const int      healRange()                 const;
	const int      getDistanceSqToUnit(const Unit & u, const TimeType & gameTime) const;
	const int      getDistanceSqToPosition(const Position & p, const TimeType & gameTime) const;
    const Position &        currentPosition(const TimeType & gameTime) const;
    void                    setPreviousPosition(const TimeType & gameTime);

    // health and damage related functions
    const HealthType        damageTakenFrom(const Unit & attacker) const;
	const HealthType        damage()                    const;
	const HealthType	    maxHP()                     const;
	const HealthType	    currentHP()                 const;
	const HealthType	    maxEnergy()                 const;
    const HealthType        getArmor()                  const;
	const float			    dpf()                       const;
	void                    updateCurrentHP(const HealthType & newHP);
    const BWAPI::UnitSizeType getSize()                 const;
    const BWAPI::WeaponType getWeapon(BWAPI::UnitType target) const;
    const PlayerWeapon      getWeapon(const Unit & target) const;

    // time and cooldown related functions
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
};

}