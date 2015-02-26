#pragma once

#include "Common.h"
#include "Array.hpp"
#include "PrerequisiteSet.h"
#include <set>

namespace BOSS
{
    
class ActionTypeData
{
public:

    // types of actions
    enum { UnitType,UpgradeType,TechType,Error };

private:
    
    static std::vector< std::vector<ActionTypeData> > allActionTypeDataVec;
    //static std::vector<DependencyGraph> dependencyGraphs;
    
    ActionID        	actionID;			// unique identifier to this action
    RaceID              raceID;

    int                 type;				// the enum type of action this is
    BWAPI::Race         race;				// the race of this action
    BWAPI::UnitType     unit;				// holds UnitType, if it's a unit
    BWAPI::UpgradeType  upgrade;			// holds UpgradeType, if it's an upgrade
    BWAPI::TechType     tech;				// holds TechType, if it's a tech

    ResourceCountType	mineralPriceVal; 	// mineral price of the action
    ResourceCountType	gasPriceVal; 		// gas price of the action
    SupplyCountType		supplyRequiredVal; 	// supply price of the action
    SupplyCountType		supplyProvidedVal;	// supply created by the action
    FrameCountType		buildTimeVal;		// build time of the action
    UnitCountType		numberProduced;		// the number of units produced

    std::string         name;				// name of the action
    std::string         shortName;
    std::string         metaName;			// meta name of this action, adds 'repeat' to the string

    PrerequisiteSet     prerequisites;
    PrerequisiteSet     recursivePrerequisites;

    bool                building; // is this a building
    bool                worker; // is this a worker
    bool                refinery; // is this a refinery
    bool                resourceDepot; // is this a resource depot
    bool                supplyProvider; // is this a supply provider
    bool                canProduceBool;
    bool                canAttackBool;
    bool                whatBuildsIsBuildingBool; // is what builds this action a building?
    bool                whatBuildsIsLarvaBool;
    bool                addon;
    bool                reqAddon;
    bool                morphed;

    ActionID            reqAddonID;

    BWAPI::UnitType     whatBuildsUnitType;		// the unit type that builds this

    ActionID            whatBuildsActionID;	// the action that builds this 

    // Private Constructors
    ActionTypeData();
    ActionTypeData(BWAPI::UnitType t, const ActionID id);
    ActionTypeData(BWAPI::UpgradeType t, const ActionID id);
    ActionTypeData(BWAPI::TechType t, const ActionID id);

    static void         AddActions();
    static void         CalculateWhatBuilds();
    static void         AddPrerequisites();

    static PrerequisiteSet CalculatePrerequisites(const ActionTypeData & action);
    static void CalculateRecursivePrerequisites(PrerequisiteSet & allPre, const ActionTypeData & action);

    static ActionTypeData   GetActionTypeData(const BWAPI::UnitType & a);
    static ActionTypeData   GetActionTypeData(const BWAPI::TechType & a);
    static ActionTypeData   GetActionTypeData(const BWAPI::UpgradeType & a);
    
    void setWhatBuildsActionID(const ActionID a,const bool wbib,const bool wbil);
    void setShortName();

public:

    static void                     Init();
    static const ActionID           GetNumActionTypes(const RaceID race);
    static const ActionTypeData &   GetActionTypeData(const RaceID raceID,const ActionID & id);
    static const RaceID             GetRaceID(BWAPI::Race r);

    RaceID              getRaceID()             const;
    ActionID            getActionID()           const;

    ActionType          whatBuildsActionType()  const;

    BWAPI::UnitType		getUnitType()			const;
    BWAPI::UnitType		whatBuildsBWAPI()       const;
    BWAPI::UpgradeType	getUpgradeType()		const;
    BWAPI::TechType		getTechType()			const;


    static ActionID     GetActionID(const BWAPI::UnitType & a);
    static ActionID     GetActionID(const BWAPI::TechType & a);
    static ActionID     GetActionID(const BWAPI::UpgradeType & a);

    ActionID            whatBuildsAction()		const;
    const PrerequisiteSet & getPrerequisites()  const;
    const PrerequisiteSet & getRecursivePrerequisites()  const;
    int 			    getType() 				const;

    const std::string & getName() 				const;
    const std::string & getShortName()          const;
    const std::string & getMetaName() 			const;

    FrameCountType 		buildTime() 			const;
    ResourceCountType	mineralPrice()			const;
    ResourceCountType	mineralPriceScaled()	const;
    ResourceCountType	gasPrice()				const;
    ResourceCountType	gasPriceScaled()		const;
    SupplyCountType 	supplyRequired() 		const;
    SupplyCountType		supplyProvided() 		const;
    UnitCountType 		numProduced() 			const;

    bool				isRefinery() 			const;
    bool				isWorker() 				const;
    bool				isBuilding()			const;
    bool				isResourceDepot()		const;
    bool				isSupplyProvider()		const;
    bool				isUnit()				const;
    bool				isTech()				const;
    bool				isUpgrade()				const;
    bool                isAddon()               const;
    bool                isMorphed()             const;
    bool                requiresAddon()         const;
    ActionID            requiredAddonID()       const;

    bool				whatBuildsIsBuilding() 	const;
    bool				whatBuildsIsLarva() 	const;
    bool				canProduce()			const;
    bool				canAttack()				const;

    void 				setPrerequisites(const PrerequisiteSet & pre);
    void 				setRecursivePrerequisites(const PrerequisiteSet & pre);
    void                setRequiredAddon(bool requiresAddon, const ActionID & id);

    bool operator ==	(const ActionTypeData & a) const;
    bool operator <		(const ActionTypeData & a) const;
};
}
