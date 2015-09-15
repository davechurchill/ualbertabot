#include "MetaType.h"

using namespace UAlbertaBot;

MetaType::MetaType () 
    : _type(MetaTypes::Default) 
    , _race(BWAPI::Races::None)
{
}

MetaType::MetaType(const std::string & name)
    : _type(MetaTypes::Default) 
    , _race(BWAPI::Races::None)
{
    std::string inputName(name);
    std::replace(inputName.begin(), inputName.end(), '_', ' ');

    for (const BWAPI::UnitType & unitType : BWAPI::UnitTypes::allUnitTypes())
    {
        // check to see if the names match exactly
        std::string typeName = unitType.getName();
        std::replace(typeName.begin(), typeName.end(), '_', ' ');
        if (typeName == inputName)
        {
            *this = MetaType(unitType);
            return;
        }

        // check to see if the names match without the race prefix
        const std::string & raceName = unitType.getRace().getName();
        if ((typeName.length() > raceName.length()) && (typeName.compare(raceName.length() + 1, typeName.length(), inputName) == 0))
        {
            *this = MetaType(unitType);
            return;
        }
    }

    for (const BWAPI::TechType & techType : BWAPI::TechTypes::allTechTypes())
    {
        std::string typeName = techType.getName();
        std::replace(typeName.begin(), typeName.end(), '_', ' ');
        if (typeName == inputName)
        {
            *this = MetaType(techType);
            return;
        }
    }

    for (const BWAPI::UpgradeType & upgradeType : BWAPI::UpgradeTypes::allUpgradeTypes())
    {
        std::string typeName = upgradeType.getName();
        std::replace(typeName.begin(), typeName.end(), '_', ' ');
        if (typeName == inputName)
        {
            *this = MetaType(upgradeType);
            return;
        }
    }

    UAB_ASSERT_WARNING(false, "Could not find MetaType with name: %s", name.c_str());
}

MetaType::MetaType (BWAPI::UnitType t) 
    : _unitType(t)
    , _type(MetaTypes::Unit) 
    , _race(t.getRace())
{
}

MetaType::MetaType (BWAPI::TechType t) 
    : _techType(t)
    , _type(MetaTypes::Tech) 
    , _race(t.getRace())
{
}

MetaType::MetaType (BWAPI::UpgradeType t) 
    : _upgradeType(t)
    , _type(MetaTypes::Upgrade) 
    , _race(t.getRace())
{
}

const size_t & MetaType::type() const
{
    return _type;
}

bool MetaType::isUnit() const 
{
    return _type == MetaTypes::Unit; 
}

bool MetaType::isTech() const 
{ 
    return _type == MetaTypes::Tech; 
}

bool MetaType::isUpgrade() const 
{ 
    return _type == MetaTypes::Upgrade; 
}

bool MetaType::isCommand() const 
{ 
    return _type == MetaTypes::Command; 
}

const BWAPI::Race & MetaType::getRace() const
{
    return _race;
}

bool MetaType::isBuilding()	const 
{ 
    return _type == MetaTypes::Unit && _unitType.isBuilding(); 
}

bool MetaType::isRefinery()	const 
{ 
    return isBuilding() && _unitType.isRefinery(); 
}

const BWAPI::UnitType & MetaType::getUnitType() const
{
    return _unitType;
}

const BWAPI::TechType & MetaType::getTechType() const
{
    return _techType;
}

const BWAPI::UpgradeType & MetaType::getUpgradeType() const
{
    return _upgradeType;
}

int MetaType::supplyRequired()
{
	if (isUnit())
	{
		return _unitType.supplyRequired();
	}
	else
	{
		return 0;
	}
}

int MetaType::mineralPrice() const
{
	return isUnit() ? _unitType.mineralPrice() : (isTech() ? _techType.mineralPrice() : _upgradeType.mineralPrice());
}

int MetaType::gasPrice() const
{
	return isUnit() ? _unitType.gasPrice() : (isTech() ? _techType.gasPrice() : _upgradeType.gasPrice());
}

BWAPI::UnitType MetaType::whatBuilds() const
{
	return isUnit() ? _unitType.whatBuilds().first : (isTech() ? _techType.whatResearches() : _upgradeType.whatUpgrades());
}

std::string MetaType::getName() const
{
	if (isUnit())
	{
		return _unitType.getName();
	}
	else if (isTech())
	{
		return _techType.getName();
	}
	else if (isUpgrade())
	{
		return _upgradeType.getName();
	}
	else
	{
		UAB_ASSERT(false, "MetaType not found");
		return "LOL";	
	}
}