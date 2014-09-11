#pragma once

#include "Common.h"
#include "PlayerProperties.h"
#include "WeaponProperties.h"
#include <boost/foreach.hpp>

namespace SparCraft
{
class WeaponProperties;
class PlayerProperties;

class UnitProperties
{
	static UnitProperties		props[256];

	BWAPI::UnitType				type;

	BWAPI::UpgradeType			capacityUpgrade;
	BWAPI::UpgradeType			extraArmorUpgrade;
	BWAPI::UpgradeType			maxEnergyUpgrade;
	BWAPI::UpgradeType			sightUpgrade;
	BWAPI::UpgradeType			speedUpgrade;

	int							capacity[2];
	int							extraArmor[2];
	int							maxEnergy[2];
	int							sightRange[2];
	int							speed[2];

	void						SetCapacityUpgrade(BWAPI::UpgradeType upgrade, int capacity0, int capacity1);
	void						SetEnergyUpgrade(BWAPI::UpgradeType upgrade);
	void						SetExtraArmorUpgrade(BWAPI::UpgradeType upgrade, int amount);
	void						SetSightUpgrade(BWAPI::UpgradeType upgrade, int range);
	void						SetSpeedUpgrade(BWAPI::UpgradeType upgrade, double rate);
	void						SetType(BWAPI::UnitType type);

public:
								UnitProperties();

	int							GetArmor(const PlayerProperties & player) const		{ return type.armor() + player.GetUpgradeLevel(type.armorUpgrade()) + extraArmor[player.GetUpgradeLevel(extraArmorUpgrade)]; }
	int							GetCapacity(const PlayerProperties & player) const	{ return capacity[player.GetUpgradeLevel(capacityUpgrade)]; }
	int							GetMaxEnergy(const PlayerProperties & player) const	{ return maxEnergy[player.GetUpgradeLevel(maxEnergyUpgrade)]; }
	int							GetSight(const PlayerProperties & player) const		{ return sightRange[player.GetUpgradeLevel(sightUpgrade)]; }
	int							GetSpeed(const PlayerProperties & player) const		{ return speed[player.GetUpgradeLevel(speedUpgrade)]; }

	const WeaponProperties &			GetGroundWeapon() const						{ return WeaponProperties::Get(type.groundWeapon()); }
	const WeaponProperties &			GetAirWeapon() const						{ return WeaponProperties::Get(type.airWeapon()); }

	static const UnitProperties &	Get(BWAPI::UnitType type)					{ return props[type.getID()]; }
	static void					Init();
};
}