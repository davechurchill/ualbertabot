#pragma once

#include "../Common.h"

namespace UAlbertaBot
{
struct MetaType 
{
	enum type_enum {Unit, Tech, Upgrade, Command, Default};
	type_enum type;

	BWAPI::UnitCommandType commandType;
	BWAPI::UnitType unitType;
	BWAPI::TechType techType;
	BWAPI::UpgradeType upgradeType;

	MetaType () : type(MetaType::Default) {}
	MetaType (BWAPI::UnitType t) :        unitType(t),    type(MetaType::Unit) {}
	MetaType (BWAPI::TechType t) :        techType(t),    type(MetaType::Tech) {}
	MetaType (BWAPI::UpgradeType t) :     upgradeType(t), type(MetaType::Upgrade) {}
	MetaType (BWAPI::UnitCommandType t) : commandType(t), type(MetaType::Command) {}

	bool isUnit()		const { return type == Unit; }
	bool isTech()		const { return type == Tech; }
	bool isUpgrade()	const { return type == Upgrade; }
	bool isCommand()	const { return type == Command; }
	bool isBuilding()	const { return type == Unit && unitType.isBuilding(); }
	bool isRefinery()	const { return isBuilding() && unitType.isRefinery(); }

	int supplyRequired()
	{
		if (isUnit())
		{
			return unitType.supplyRequired();
		}
		else
		{
			return 0;
		}
	}

	int mineralPrice() const
	{
		return isUnit() ? unitType.mineralPrice() : (isTech() ? techType.mineralPrice() : upgradeType.mineralPrice());
	}

	int gasPrice() const
	{
		return isUnit() ? unitType.gasPrice() : (isTech() ? techType.gasPrice() : upgradeType.gasPrice());
	}

	BWAPI::UnitType whatBuilds() const
	{
		return isUnit() ? unitType.whatBuilds().first : (isTech() ? techType.whatResearches() : upgradeType.whatUpgrades());
	}

	std::string getName() const
	{
		if (isUnit())
		{
			return unitType.getName();
		}
		else if (isTech())
		{
			return techType.getName();
		}
		else if (isUpgrade())
		{
			return upgradeType.getName();
		}
		else if (isCommand())
		{
			return commandType.getName();
		}
		else
		{
			UAB_ASSERT(false, "MetaType not found");
			return "LOL";	
		}
	}
};
}