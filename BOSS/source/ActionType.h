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
    ActionType(const BWAPIDATA::UnitType & type);
    ActionType(const BWAPIDATA::UpgradeType & type);
    ActionType(const BWAPIDATA::TechType & type);

    ActionType & operator = (const ActionType & rhs);

    const ActionID              ID()                    const;
    const RaceID                getRace()               const;

    BWAPIDATA::UnitType             getUnitType()           const;
	BWAPIDATA::UnitType             whatBuildsBWAPI()       const;
    ActionType                  whatBuildsActionType()  const;
	BWAPIDATA::UpgradeType          getUpgradeType()        const;
	BWAPIDATA::TechType             getTechType()           const;
	
	ActionID                    whatBuildsAction()      const;	
	const PrerequisiteSet &     getPrerequisites()      const;
    const PrerequisiteSet &     getRecursivePrerequisites()      const;
	int                         getType()               const;
	
	std::string                 getName()               const;
	std::string                 getMetaName()           const;
	
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
    const bool         TypeExists(const BWAPIDATA::UnitType & a);
    const bool         TypeExists(const BWAPIDATA::TechType & a);
    const bool         TypeExists(const BWAPIDATA::UpgradeType & a);

    extern ActionType None;

}
}