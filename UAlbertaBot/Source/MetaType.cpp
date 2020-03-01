#include "MetaType.h"

using namespace UAlbertaBot;

MetaType::MetaType ()
{
}

MetaType::MetaType(const std::string & name)
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
    : m_unitType(t)
    , m_type(MetaTypes::Unit)
    , m_race(t.getRace())
{
}

MetaType::MetaType (BWAPI::TechType t)
    : m_techType(t)
    , m_type(MetaTypes::Tech)
    , m_race(t.getRace())
{
}

MetaType::MetaType (BWAPI::UpgradeType t)
    : m_upgradeType(t)
    , m_type(MetaTypes::Upgrade)
    , m_race(t.getRace())
{
}

const size_t & MetaType::type() const
{
    return m_type;
}

bool MetaType::isUnit() const
{
    return m_type == MetaTypes::Unit;
}

bool MetaType::isTech() const
{
    return m_type == MetaTypes::Tech;
}

bool MetaType::isUpgrade() const
{
    return m_type == MetaTypes::Upgrade;
}

bool MetaType::isCommand() const
{
    return m_type == MetaTypes::Command;
}

const BWAPI::Race & MetaType::getRace() const
{
    return m_race;
}

bool MetaType::isBuilding()	const
{
    return m_type == MetaTypes::Unit && m_unitType.isBuilding();
}

bool MetaType::isRefinery()	const
{
    return isBuilding() && m_unitType.isRefinery();
}

const BWAPI::UnitType & MetaType::getUnitType() const
{
    return m_unitType;
}

const BWAPI::TechType & MetaType::getTechType() const
{
    return m_techType;
}

const BWAPI::UpgradeType & MetaType::getUpgradeType() const
{
    return m_upgradeType;
}

int MetaType::supplyRequired()
{
    if (isUnit())
    {
        return m_unitType.supplyRequired();
    }
    else
    {
        return 0;
    }
}

int MetaType::mineralPrice() const
{
    return isUnit() ? m_unitType.mineralPrice() : (isTech() ? m_techType.mineralPrice() : m_upgradeType.mineralPrice());
}

int MetaType::gasPrice() const
{
    return isUnit() ? m_unitType.gasPrice() : (isTech() ? m_techType.gasPrice() : m_upgradeType.gasPrice());
}

BWAPI::UnitType MetaType::whatBuilds() const
{
    return isUnit() ? m_unitType.whatBuilds().first : (isTech() ? m_techType.whatResearches() : m_upgradeType.whatUpgrades());
}

std::string MetaType::getName() const
{
    if (isUnit())
    {
        return m_unitType.getName();
    }
    else if (isTech())
    {
        return m_techType.getName();
    }
    else if (isUpgrade())
    {
        return m_upgradeType.getName();
    }
    else
    {
        UAB_ASSERT(false, "MetaType not found");
        return "LOL";
    }
}