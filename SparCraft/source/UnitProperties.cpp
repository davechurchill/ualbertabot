#include "UnitProperties.h"

using namespace SparCraft;
using namespace BWAPI::UpgradeTypes;

UnitProperties UnitProperties::props[256];

UnitProperties::UnitProperties() : 
	capacityUpgrade(BWAPI::UpgradeTypes::None),
	maxEnergyUpgrade(BWAPI::UpgradeTypes::None),
	sightUpgrade(BWAPI::UpgradeTypes::None),
	extraArmorUpgrade(BWAPI::UpgradeTypes::None),
	speedUpgrade(BWAPI::UpgradeTypes::None)
{
	capacity[0]		= capacity[1]		= 0;
}

void UnitProperties::SetType(BWAPI::UnitType type)
{
	this->type		= type;
	maxEnergy[0]	= maxEnergy[1]		= type.maxEnergy();
	sightRange[0]	= sightRange[1]		= type.sightRange() << pixelShift;
	extraArmor[0]	= extraArmor[1]		= 0;
	speed[0]		= speed[1]			= static_cast<int>((1 << pixelShift) * type.topSpeed());
}

void UnitProperties::SetSpeedUpgrade(BWAPI::UpgradeType upgrade, double rate)
{
	speedUpgrade				= upgrade;
	speed[1]					= static_cast<int>((1 << pixelShift) * rate);
}

void UnitProperties::SetCapacityUpgrade(BWAPI::UpgradeType upgrade, int capacity0, int capacity1)
{
	capacityUpgrade				= upgrade;
	capacity[0]					= capacity0;
	capacity[1]					= capacity1;
}

void UnitProperties::SetEnergyUpgrade(BWAPI::UpgradeType upgrade)
{
	maxEnergyUpgrade			= upgrade;
	maxEnergy[1]				= 250;
}

void UnitProperties::SetSightUpgrade(BWAPI::UpgradeType upgrade, int range)
{
	sightUpgrade				= upgrade;
	sightRange[1]				= (range << 5) << pixelShift;
}

void UnitProperties::SetExtraArmorUpgrade(BWAPI::UpgradeType upgrade, int amount)
{
	extraArmorUpgrade			= upgrade;
	extraArmor[1]				= amount;
}

using namespace BWAPI::UnitTypes;

void UnitProperties::Init()
{
	BOOST_FOREACH (const BWAPI::UnitType & type, BWAPI::UnitTypes::allUnitTypes())
	{
		props[type.getID()].SetType(type);
	}

	const double standardSpeed(Terran_SCV.topSpeed());

	props[BWAPI::UnitTypes::Terran_Ghost.getID()            ].SetEnergyUpgrade(Moebius_Reactor);
	props[BWAPI::UnitTypes::Terran_Ghost.getID()            ].SetSightUpgrade(Ocular_Implants, 11);

	props[BWAPI::UnitTypes::Terran_Medic.getID()            ].SetEnergyUpgrade(Caduceus_Reactor);

	props[BWAPI::UnitTypes::Terran_Vulture.getID()          ].SetSpeedUpgrade(Ion_Thrusters,            standardSpeed * 1.881);

	props[BWAPI::UnitTypes::Terran_Wraith.getID()           ].SetEnergyUpgrade(Apollo_Reactor);

	props[BWAPI::UnitTypes::Terran_Battlecruiser.getID()    ].SetEnergyUpgrade(Colossus_Reactor);
	props[BWAPI::UnitTypes::Terran_Science_Vessel.getID()   ].SetEnergyUpgrade(Titan_Reactor);



	props[BWAPI::UnitTypes::Zerg_Zergling.getID()       ].SetSpeedUpgrade(Metabolic_Boost,			    standardSpeed * 1.615);

	props[BWAPI::UnitTypes::Zerg_Hydralisk.getID()      ].SetSpeedUpgrade(Muscular_Augments,		    standardSpeed * 1.105);

	props[BWAPI::UnitTypes::Zerg_Ultralisk.getID()      ].SetExtraArmorUpgrade(Chitinous_Plating,	    2);
	props[BWAPI::UnitTypes::Zerg_Ultralisk.getID()      ].SetSpeedUpgrade(Anabolic_Synthesis,		    standardSpeed * 1.556);

	props[BWAPI::UnitTypes::Zerg_Defiler.getID()        ].SetEnergyUpgrade(Metasynaptic_Node);

	props[BWAPI::UnitTypes::Zerg_Overlord.getID()       ].SetSightUpgrade(Antennae,					    11);
	props[BWAPI::UnitTypes::Zerg_Overlord.getID()       ].SetSpeedUpgrade(Pneumatized_Carapace,		    Protoss_Carrier.topSpeed());

	props[BWAPI::UnitTypes::Zerg_Queen.getID()          ].SetEnergyUpgrade(Gamete_Meiosis);



	props[BWAPI::UnitTypes::Protoss_Zealot.getID()      ].SetSpeedUpgrade(Leg_Enhancements,			    standardSpeed * 1.167);

	props[BWAPI::UnitTypes::Protoss_High_Templar.getID()].SetEnergyUpgrade(Khaydarin_Amulet);

	props[BWAPI::UnitTypes::Protoss_Reaver.getID()      ].SetCapacityUpgrade(Reaver_Capacity,		    5, 10);

	props[BWAPI::UnitTypes::Protoss_Dark_Archon.getID() ].SetEnergyUpgrade(Argus_Talisman);

	props[BWAPI::UnitTypes::Protoss_Observer.getID()    ].SetSightUpgrade(Sensor_Array,				    11);
	props[BWAPI::UnitTypes::Protoss_Observer.getID()    ].SetSpeedUpgrade(Gravitic_Boosters,		    Protoss_Corsair.topSpeed());

	props[BWAPI::UnitTypes::Protoss_Shuttle.getID()     ].SetSpeedUpgrade(Gravitic_Drive,			    Protoss_Corsair.topSpeed());

	props[BWAPI::UnitTypes::Protoss_Scout.getID()       ].SetSightUpgrade(Apial_Sensors,			    10);
	props[BWAPI::UnitTypes::Protoss_Scout.getID()       ].SetSpeedUpgrade(Gravitic_Thrusters,		    Protoss_Corsair.topSpeed());

	props[BWAPI::UnitTypes::Protoss_Corsair.getID()     ].SetEnergyUpgrade(Argus_Jewel);

    props[BWAPI::UnitTypes::Protoss_Carrier.getID()     ].SetCapacityUpgrade(Carrier_Capacity,		    4, 8);

	props[BWAPI::UnitTypes::Protoss_Arbiter.getID()     ].SetEnergyUpgrade(Khaydarin_Core);
}