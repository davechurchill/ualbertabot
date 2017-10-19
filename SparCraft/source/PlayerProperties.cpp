#include "PlayerProperties.h"
#include "WeaponProperties.h"
#include "SparCraftAssert.h"

using namespace SparCraft;

PlayerProperties PlayerProperties::props[2];

PlayerProperties::PlayerProperties()
{
	Reset();
}

PlayerProperties::PlayerProperties(const BWAPI::Player & player) 
{ 
	Capture(player); 
}

PlayerProperties & PlayerProperties::Get(const size_t & playerID)
{ 
    return props[playerID]; 
}

void PlayerProperties::Reset()
{
	for(int i(0); i<NUM_UPGRADES; ++i)
	{
		upgradeLevel[i] = 0;
	}

	for(int i(0); i<NUM_TECHS; ++i)
	{
		hasResearched[i] = false;
	}
}

void PlayerProperties::SetUpgradeLevel(BWAPI::UpgradeType upgrade, int level)
{
	SPARCRAFT_ASSERT(upgrade != BWAPI::UpgradeTypes::None, "Bad Upgrade Type");
	SPARCRAFT_ASSERT(upgrade != BWAPI::UpgradeTypes::Unknown, "Bad Upgrade Type");
	SPARCRAFT_ASSERT(level >= 0 && level <= upgrade.maxRepeats(), "Bad Upgrade Level");
	upgradeLevel[upgrade.getID()] = level;
}

void PlayerProperties::SetResearched(BWAPI::TechType tech, bool researched)
{
	SPARCRAFT_ASSERT(tech != BWAPI::TechTypes::None, "Bad Tech Type"); 
	SPARCRAFT_ASSERT(tech != BWAPI::TechTypes::Unknown, "Bad Tech Type"); 
	hasResearched[tech.getID()] = researched;
}

void PlayerProperties::Capture(const BWAPI::Player & player)
{
	for(int i(0); i<NUM_UPGRADES; ++i)
	{
		upgradeLevel[i] = player->getUpgradeLevel(i);
	}

	for(int i(0); i<NUM_TECHS; ++i)
	{
		hasResearched[i] = player->hasResearched(i);
	}
}

int PlayerProperties::GetUpgradeLevel(BWAPI::UpgradeType upgrade) const 
{ 
    return upgradeLevel[upgrade.getID()]; 
}

bool PlayerProperties::HasUpgrade(BWAPI::UpgradeType upgrade) const 
{ 
    return upgradeLevel[upgrade.getID()] > 0; 
}

bool PlayerProperties::HasResearched(BWAPI::TechType tech) const 
{ 
    return hasResearched[tech.getID()]; 
}

PlayerWeapon::PlayerWeapon(const PlayerProperties * player, BWAPI::WeaponType type) 
    : player(player)
    , type(type) 
{
}

int	PlayerWeapon::GetDamageBase() const										
{ 
    return SparCraft::WeaponProperties::Get(type).GetDamageBase(*player); 
}

float PlayerWeapon::GetDamageMultiplier(BWAPI::UnitSizeType targetSize) const	
{ 
    return SparCraft::WeaponProperties::Get(type).GetDamageMultiplier(targetSize); 
}

int	PlayerWeapon::GetCooldown() const											
{ 
    return SparCraft::WeaponProperties::Get(type).GetCooldown(*player); 
}

int	PlayerWeapon::GetMaxRange() const
{ 
    return SparCraft::WeaponProperties::Get(type).GetMaxRange(*player); 
}