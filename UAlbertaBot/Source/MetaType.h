#pragma once

#include "Common.h"

namespace UAlbertaBot
{

namespace MetaTypes
{
    enum {Unit, Tech, Upgrade, Command, Default};
}

class MetaType 
{
	size_t                  _type;
    BWAPI::Race             _race;

	BWAPI::UnitType         _unitType;
	BWAPI::TechType         _techType;
	BWAPI::UpgradeType      _upgradeType;

public:

	MetaType ();
    MetaType (const std::string & name);
	MetaType (BWAPI::UnitType t);
	MetaType (BWAPI::TechType t);
	MetaType (BWAPI::UpgradeType t);

	bool    isUnit()		const;
	bool    isTech()		const;
	bool    isUpgrade()	    const;
	bool    isCommand()	    const;
	bool    isBuilding()	const;
	bool    isRefinery()	const;
    
    const size_t & type() const;
    const BWAPI::Race & getRace() const;

    const BWAPI::UnitType & getUnitType() const;
    const BWAPI::TechType & getTechType() const;
    const BWAPI::UpgradeType & getUpgradeType() const;

	int     supplyRequired();
	int     mineralPrice()  const;
	int     gasPrice()      const;

	BWAPI::UnitType whatBuilds() const;
	std::string getName() const;
};
}