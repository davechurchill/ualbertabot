#include "WeaponProperties.h"

using namespace SparCraft;
using namespace BWAPI::UpgradeTypes;


namespace SparCraft
{
	const int pixelShift(10);
	const float damageMultipliers[7][6] = 
	{ 
		{0,  0,   0,    0,    0,  0}, 
		{0,  .5f, .75f, 1,    0,  0}, 
		{0,  1,   .5f,  .25,  0,  0},
		{0,  1,   1,    1,    0,  0},
		{0,  1,   1,    1,    0,  0},
		{0,  0,   0,    0,    0,  0}, 
		{0,  0,   0,    0,    0,  0}
	};

	WeaponProperties WeaponProperties::props[256];
}

WeaponProperties::WeaponProperties() :
    rangeUpgrade(BWAPI::UpgradeTypes::None),
    speedUpgrade(BWAPI::UpgradeTypes::None)
{

}

void WeaponProperties::SetType(BWAPI::WeaponType type)
{
	this->type		= type;
	cooldown[0]		= type.damageCooldown();
    cooldown[1]		= type.damageCooldown();
	maxRange[0]		= type.maxRange();
    maxRange[1]		= type.maxRange();
}

void WeaponProperties::SetRangeUpgrade(BWAPI::UpgradeType upgrade, int maxRange)
{
	rangeUpgrade		= upgrade;
	this->maxRange[1]	= (maxRange << 5);
}

void WeaponProperties::SetSpeedUpgrade(BWAPI::UpgradeType upgrade, int cooldown)
{
	speedUpgrade		= upgrade;
	this->cooldown[1]	= cooldown;
}

using namespace BWAPI::WeaponTypes;

void WeaponProperties::Init()
{
    BOOST_FOREACH (const BWAPI::WeaponType & type, BWAPI::WeaponTypes::allWeaponTypes())
	{
		props[type.getID()].SetType(type);
	}

	props[Gauss_Rifle.getID()			].SetRangeUpgrade(U_238_Shells,			5);	// Terran Marine ground/air attack

	props[Hellfire_Missile_Pack.getID()	].SetRangeUpgrade(Charon_Boosters,		8);	// Terran Goliath air attack

	props[Claws.getID()					].SetSpeedUpgrade(Adrenal_Glands,		6);	// Zerg Zergling ground attack

	props[Needle_Spines.getID()			].SetRangeUpgrade(Grooved_Spines,		5); // Zerg Hydralisk ground/air attack

	props[Phase_Disruptor.getID()		].SetRangeUpgrade(Singularity_Charge,	6);	// Protoss Dragoon ground/air attack
}

int WeaponProperties::GetDamageBase(const PlayerProperties & player) const 
{ 
    return type.damageAmount() + player.GetUpgradeLevel(type.upgradeType()) * type.damageFactor(); 
}

float WeaponProperties::GetDamageMultiplier(BWAPI::UnitSizeType targetSize) const 
{ 
    return damageMultipliers[type.damageType().getID()][targetSize.getID()]; 
}

int WeaponProperties::GetCooldown(const PlayerProperties & player) const 
{ 
    return cooldown[player.GetUpgradeLevel(speedUpgrade)]; 
}

int WeaponProperties::GetMaxRange(const PlayerProperties & player) const 
{ 
    return maxRange[player.GetUpgradeLevel(rangeUpgrade)];
}

const WeaponProperties & WeaponProperties::Get(BWAPI::WeaponType type) 
{ 
    return props[type.getID()]; 
}