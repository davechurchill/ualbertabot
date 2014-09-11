#pragma once

#include "Common.h"
#include "WeaponProperties.h"

namespace SparCraft
{
	
class PlayerProperties
{
	enum        { NUM_UPGRADES	= 63, NUM_TECHS	= 47 };

	int			upgradeLevel[NUM_UPGRADES];
	bool		hasResearched[NUM_TECHS];

    static      PlayerProperties    props[2];

public:
				PlayerProperties();
				PlayerProperties(const BWAPI::Player & player);

	int			GetUpgradeLevel(BWAPI::UpgradeType upgrade) const;
	bool		HasUpgrade(BWAPI::UpgradeType upgrade) const;
	bool		HasResearched(BWAPI::TechType tech) const;

	void		Reset();
	void		SetUpgradeLevel(BWAPI::UpgradeType upgrade, int level);
	void		SetResearched(BWAPI::TechType tech, bool researched);
	void		Capture(const BWAPI::Player & player);

    static      PlayerProperties & Get(const size_t & playerID);
};

class PlayerWeapon
{
	const PlayerProperties *		player;
	BWAPI::WeaponType	type;

public:
			PlayerWeapon(const PlayerProperties * player, BWAPI::WeaponType type);

	int		GetDamageBase() const;
	float	GetDamageMultiplier(BWAPI::UnitSizeType targetSize) const;
	int		GetCooldown() const;
	int		GetMaxRange() const;
};
}
