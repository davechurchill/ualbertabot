#pragma once

#include "Location.hpp"

class Unit 
{
	int		_damage,
			_maxHP,
			_currentHP,
			_range,
			_moveCooldown,
			_weaponCooldown,
			_lastMove,
			_lastAttack;

public:

	Unit()
		 : _damage(0)
		 , _maxHP(0)
		 , _currentHP(0)
		 , _range(0)
		 , _moveCooldown(0)
		 , _weaponCooldown(0)
		 , _lastMove(-1)
		 , _lastAttack(-1)
	{

	}

	Unit(const int & damage, const int & maxHP, const int & currentHP, 
		 const int & range, const int & moveCooldown, const int & weaponCooldown) 
		 : _damage(damage)
		 , _maxHP(maxHP)
		 , _currentHP(currentHP)
		 , _range(range)
		 , _moveCooldown(moveCooldown)
		 , _weaponCooldown(weaponCooldown)
		 , _lastMove(-1)
		 , _lastAttack(-1)
	{

	}

	const int damage()			const { return _damage; }
	const int maxHP()			const { return _maxHP; }
	const int currentHP()		const { return _currentHP; }
	const int range()			const { return _range; }
	const int moveCooldown()	const { return _moveCooldown; }
	const int weaponCooldown()	const { return _weaponCooldown; }
	const int lastMove()		const { return _lastMove; }
	const int lastAttack()		const { return _lastAttack; }
};
