#pragma once

#include "Common.h"

namespace UAlbertaBot
{

namespace MetaTypes
{
enum { Unit, Tech, Upgrade, Command, Default };
}
class MetaType
{
    size_t              m_type = MetaTypes::Default;
    BWAPI::Race         m_race = BWAPI::Races::None;

    BWAPI::UnitType     m_unitType;
    BWAPI::TechType     m_techType;
    BWAPI::UpgradeType  m_upgradeType;

public:

    MetaType ();
    MetaType (const std::string & name);
    MetaType (BWAPI::UnitType t);
    MetaType (BWAPI::TechType t);
    MetaType (BWAPI::UpgradeType t);

    bool isUnit()       const;
    bool isTech()       const;
    bool isUpgrade()    const;
    bool isCommand()    const;
    bool isBuilding()   const;
    bool isRefinery()   const;

    const size_t & type() const;
    const BWAPI::Race & getRace() const;

    const BWAPI::UnitType & getUnitType() const;
    const BWAPI::TechType & getTechType() const;
    const BWAPI::UpgradeType & getUpgradeType() const;

    int supplyRequired();
    int mineralPrice() const;
    int gasPrice() const;

    BWAPI::UnitType whatBuilds() const;
    std::string getName() const;
};

typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

}