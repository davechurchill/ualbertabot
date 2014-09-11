#pragma once

#include "Common.h"
#include "PlayerProperties.h"
#include <boost/foreach.hpp>

namespace SparCraft
{
    
class PlayerProperties;

extern const int				pixelShift;
extern const float				damageMultipliers[7][6];

class WeaponProperties
{
	static WeaponProperties		props[256];

	BWAPI::WeaponType			type;

	BWAPI::UpgradeType			rangeUpgrade;
	BWAPI::UpgradeType			speedUpgrade;

	int							cooldown[2];
	int							maxRange[2];

	void						SetRangeUpgrade(BWAPI::UpgradeType upgrade, int maxRange);
	void						SetSpeedUpgrade(BWAPI::UpgradeType upgrade, int cooldown);
	void						SetType(BWAPI::WeaponType type);
public:
								WeaponProperties();

	int							GetDamageBase(const PlayerProperties & player) const;
	float						GetDamageMultiplier(BWAPI::UnitSizeType targetSize) const;
	int							GetCooldown(const PlayerProperties & player) const;
	int							GetMaxRange(const PlayerProperties & player) const;

	static const WeaponProperties &	Get(BWAPI::WeaponType type);
	static void					Init();
};
}
