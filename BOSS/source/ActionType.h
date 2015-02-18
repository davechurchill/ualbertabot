#pragma once

#include "Common.h"

namespace BOSS
{

class PrerequisiteSet;

class ActionType
{
    const ActionID	    _id;
    const RaceID        _race;

public:
	
    ActionType();
    ActionType(const RaceID & race, const ActionID & id);
    ActionType(const ActionType & type);
    ActionType(const BWAPI::UnitType & type);
    ActionType(const BWAPI::UpgradeType & type);
    ActionType(const BWAPI::TechType & type);

    ActionType & operator = (const ActionType & rhs);

    const ActionID              ID()                    const;
    const RaceID                getRace()               const;

    BWAPI::UnitType             getUnitType()           const;
	BWAPI::UnitType             whatBuildsBWAPI()       const;
    ActionType                  whatBuildsActionType()  const;
	BWAPI::UpgradeType          getUpgradeType()        const;
	BWAPI::TechType             getTechType()           const;
	
	ActionID                    whatBuildsAction()      const;	
	const PrerequisiteSet &     getPrerequisites()      const;
    const PrerequisiteSet &     getRecursivePrerequisites()      const;
	int                         getType()               const;
	
	const std::string &         getName()               const;
    const std::string &         getShortName()          const;
	const std::string &         getMetaName()           const;
	
	FrameCountType              buildTime()             const;
	ResourceCountType           mineralPrice()          const;
	ResourceCountType           mineralPriceScaled()    const;
	ResourceCountType           gasPrice()              const;
	ResourceCountType           gasPriceScaled()        const;
	SupplyCountType             supplyRequired()        const;
	SupplyCountType             supplyProvided()        const;
	UnitCountType               numProduced()           const;

	bool                        isRefinery()            const;
	bool                        isWorker()              const;
	bool                        isBuilding()            const;
	bool                        isResourceDepot()       const;
	bool                        isSupplyProvider()      const;
	bool                        isUnit()                const;
	bool                        isTech()                const;
	bool                        isUpgrade()             const;
	bool                        whatBuildsIsBuilding()  const;
	bool                        whatBuildsIsLarva()     const;
	bool                        canProduce()            const;
	bool                        canAttack()             const;
    bool                        isAddon()               const;
    bool                        requiresAddon()         const;
    bool                        isMorphed()             const;
    bool                        canBuild(const ActionType & t) const;
    ActionType                  requiredAddonType()     const;

    const bool operator == (const ActionType & rhs)     const;
    const bool operator != (const ActionType & rhs)     const;
    const bool operator <  (const ActionType & rhs)     const;
};

class ActionSet;

namespace ActionTypes
{
    void init();
    const std::vector<ActionType> &  GetAllActionTypes(const RaceID race);
    const ActionType & GetActionType(const RaceID & race, const ActionID & id);
    const ActionType & GetWorker(const RaceID raceID);
    const ActionType & GetSupplyProvider(const RaceID raceID);
    const ActionType & GetRefinery(const RaceID raceID);
    const ActionType & GetResourceDepot(const RaceID raceID);
    const ActionType & GetActionType(const std::string & name);
    const bool         TypeExists(const std::string & name);
    const bool         TypeExists(const BWAPI::UnitType & a);
    const bool         TypeExists(const BWAPI::TechType & a);
    const bool         TypeExists(const BWAPI::UpgradeType & a);

    extern ActionType None;

}
}