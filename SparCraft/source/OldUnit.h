#pragma once

#include "Common.h"

#include "Action.h"
#include "Position.hpp"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "AnimationFrameData.h"
#include <iostream>

namespace SparCraft
{

class Unit 
{
    BWAPI::UnitType     _unitType;				// the BWAPI unit type that we are mimicing
    int        _range;
	
	Position            _position;				// current location in a possibly infinite space
	
	size_t              _unitID;				// unique unit ID to the state it's contained in
    size_t              _bwapiID;
    size_t              _playerID;				// the player who controls the unit
	
	size_t          _currentHP;				// current HP of the unit
	size_t          _currentEnergy;

	size_t            _timeCanMove;			// time the unit can next move
	size_t            _timeCanAttack;			// time the unit can next attack

	Action              _previousAction;		// the previous action that the unit performed
	size_t            _previousActionTime;	// the time the previous move was performed
	Position            _previousPosition;

    mutable size_t    _prevCurrentPosTime;
    mutable Position    _prevCurrentPos;

public:

	Unit();
	Unit(const BWAPI::UnitType unitType, const size_t & playerID, const Position & pos);
	Unit(const BWAPI::UnitType unitType, const Position & pos, const size_t & unitID, const size_t & playerID, 
		 const size_t & hp, const size_t & energy, const size_t & tm, const size_t & ta);

    // action functions
	void                    setPreviousAction(const Action & m, const size_t & previousMoveTime);
	void                    updateAttackActionTime(const size_t & newTime);
	void                    updateMoveActionTime(const size_t & newTime);
	void                    attack(const Action & move, const Unit & target, const size_t & gameTime);
	void                    heal(const Action & move, const Unit & target, const size_t & gameTime);
	void                    move(const Action & move, const size_t & gameTime) ;
	void                    waitUntilAttack(const Action & move, const size_t & gameTime);
	void                    pass(const Action & move, const size_t & gameTime);
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
	bool					canSeeTarget(const Unit & unit, const size_t & gameTime) const;
	const bool              canAttackTarget(const Unit & unit, const size_t & gameTime) const;
	const bool              canHealTarget(const Unit & unit, const size_t & gameTime) const;

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
	const int      getDistanceSqToUnit(const Unit & u, const size_t & gameTime) const;
	const int      getDistanceSqToPosition(const Position & p, const size_t & gameTime) const;
    const Position &        currentPosition(const size_t & gameTime) const;
    void                    setPreviousPosition(const size_t & gameTime);

    // health and damage related functions
	const size_t        damage()                    const;
	const size_t        healAmount()                const;
	const size_t	    maxHP()                     const;
	const size_t	    currentHP()                 const;
	const size_t	    currentEnergy()             const;
	const size_t	    maxEnergy()                 const;
	const size_t	    healCost()                  const;
    const size_t        getArmor()                  const;
	const float			    dpf()                       const;
	void                    updateCurrentHP(const size_t & newHP);
    const BWAPI::UnitSizeType getSize()                 const;
    const BWAPI::WeaponType getWeapon(BWAPI::UnitType target) const;
    const size_t        getDamageTo(const Unit & unit) const;
    const PlayerWeapon      getWeapon(const Unit & target) const;

    // time and cooldown related functions
	const size_t		    moveCooldown()              const;
	const size_t		    attackCooldown()            const;
	const size_t		    healCooldown()              const;
	const size_t		    nextAttackActionTime()      const;
	const size_t		    nextMoveActionTime()        const;
	const size_t		    previousActionTime()        const;
	const size_t		    firstTimeFree()             const;
	const size_t 		    attackInitFrameTime()       const;
	const size_t 		    attackRepeatFrameTime()     const;
	void                    setCooldown(size_t attack, size_t move);

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