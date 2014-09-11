#include "ActionTypeData.h"
#include "ActionType.h"

using namespace BOSS;

// default constructor, should never be called
ActionTypeData::ActionTypeData() 
    : type(Error) 
{ 
   
}

// UnitType constructor
ActionTypeData::ActionTypeData(BWAPI::UnitType t, const ActionID id) 
	: type                      (UnitType)
	, unit                      (t)
    , raceID                    (GetRaceID(t.getRace()))
	, actionID                  (id)
	, mineralPriceVal           (t.mineralPrice())
	, gasPriceVal               (t.gasPrice())
	, supplyRequiredVal         (t.supplyRequired())
	, supplyProvidedVal         (t.supplyProvided())
	, buildTimeVal              (t.buildTime())
	, numberProduced            (1)
	, name                      (t.getName())
	, metaName                  (t.getName())
	, building                  (t.isBuilding())
	, worker                    (t.isWorker())
	, refinery                  (t.isRefinery())
	, resourceDepot             (t.isResourceDepot())
	, supplyProvider            (t.supplyProvided() > 0 && !t.isResourceDepot())
	, canProduceBool            (t.isBuilding() && t.canProduce())
	, canAttackBool             (t.canAttack())
	, whatBuildsUnitType        (t.whatBuilds().first)
    , addon                     (t.isAddon())
    , reqAddon(false)
    , reqAddonID(0)
{
	if (t == BWAPI::UnitTypes::Zerg_Zergling || t == BWAPI::UnitTypes::Zerg_Scourge)
	{
		numberProduced = 2;
	}
}

// UpgradeType action
ActionTypeData::ActionTypeData(BWAPI::UpgradeType t, const ActionID id) 
	: type(UpgradeType)
	, upgrade(t)
	, actionID(id)
    , raceID(GetRaceID(t.getRace()))
	, mineralPriceVal(t.mineralPrice())
	, gasPriceVal(t.gasPrice())
	, supplyRequiredVal(0)
	, supplyProvidedVal(0)
	, buildTimeVal(t.upgradeTime())
	, numberProduced(1)
	, name(t.getName())
	, metaName(t.getName())
	, building(false)
	, worker(false)
	, refinery(false)
	, resourceDepot(false)
	, supplyProvider(false)
	, canProduceBool(false)
	, canAttackBool(false)
	, whatBuildsUnitType(t.whatUpgrades())
    , addon(false)
    , reqAddon(false)
    , reqAddonID(0)
{
}

// TechType action
ActionTypeData::ActionTypeData(BWAPI::TechType t, const ActionID id)
	: type(TechType)
	, tech(t)
	, actionID(id)
    , raceID(GetRaceID(t.getRace()))
	, mineralPriceVal(t.mineralPrice())
	, gasPriceVal(t.gasPrice())
	, supplyRequiredVal(0)
	, supplyProvidedVal(0)
	, buildTimeVal(t.researchTime())
	, numberProduced(1)
	, name(t.getName())
	, metaName(t.getName())
	, building(false)
	, worker(false)
	, refinery(false)
	, resourceDepot(false)
	, supplyProvider(false)
	, canProduceBool(false)
	, canAttackBool(false)
	, whatBuildsUnitType(t.whatResearches())
    , addon(false)
    , reqAddon(false)
    , reqAddonID(0)
{}

RaceID              ActionTypeData::getRaceID()             const   { return raceID; }
ActionID            ActionTypeData::getActionID()           const   { return actionID; }

BWAPI::UnitType		ActionTypeData::getUnitType()			const	{ return unit; }
BWAPI::UnitType		ActionTypeData::whatBuildsBWAPI() 		const	{ return whatBuildsUnitType; }
BWAPI::UpgradeType	ActionTypeData::getUpgradeType()		const	{ return upgrade; }
BWAPI::TechType		ActionTypeData::getTechType()			const	{ return tech; }
	
ActionID            ActionTypeData::whatBuildsAction()		const	{ return whatBuildsActionID; }	
int 			    ActionTypeData::getType() 				const	{ return type; }
	
std::string 		ActionTypeData::getName() 				const	{ return name; }
std::string 		ActionTypeData::getMetaName() 			const	{ return metaName; }
	
FrameCountType 		ActionTypeData::buildTime() 			const	{ return buildTimeVal; }
ResourceCountType	ActionTypeData::mineralPrice()			const	{ return mineralPriceVal; }
ResourceCountType	ActionTypeData::mineralPriceScaled()	const	{ return mineralPriceVal * 100; }
ResourceCountType	ActionTypeData::gasPrice()				const	{ return gasPriceVal; }
ResourceCountType	ActionTypeData::gasPriceScaled()		const	{ return gasPriceVal * 100; }
SupplyCountType 	ActionTypeData::supplyRequired() 		const	{ return supplyRequiredVal; }
SupplyCountType		ActionTypeData::supplyProvided() 		const	{ return supplyProvidedVal; }
UnitCountType 		ActionTypeData::numProduced() 			const	{ return numberProduced; }

bool				ActionTypeData::isRefinery() 			const	{ return refinery; }
bool				ActionTypeData::isWorker() 			    const	{ return worker;   }
bool				ActionTypeData::isBuilding()			const	{ return building; }
bool				ActionTypeData::isResourceDepot()		const	{ return resourceDepot; }
bool				ActionTypeData::isSupplyProvider()		const	{ return supplyProvider; }
bool				ActionTypeData::isUnit()				const	{ return type == UnitType; }
bool				ActionTypeData::isTech()				const	{ return type == TechType; }
bool				ActionTypeData::isUpgrade()			    const	{ return type == UpgradeType; }
bool				ActionTypeData::isAddon()			    const	{ return addon; }
bool                ActionTypeData::requiresAddon()         const   { return reqAddon; }
ActionID            ActionTypeData::requiredAddonID()       const   { return reqAddonID; }
	
bool				ActionTypeData::whatBuildsIsBuilding()  const	{ return whatBuildsIsBuildingBool; }
bool				ActionTypeData::whatBuildsIsLarva() 	const	{ return whatBuildsIsLarvaBool; }
bool				ActionTypeData::canProduce()			const	{ return canProduceBool; }
bool				ActionTypeData::canAttack()			    const	{ return canAttackBool; }
		
void 				ActionTypeData::setPrerequisites(const PrerequisiteSet & pre) 	{ prerequisites = pre; }
void 				ActionTypeData::setRecursivePrerequisites(const PrerequisiteSet & pre) 	{ recursivePrerequisites = pre; }

bool                ActionTypeData::operator ==	(const ActionTypeData & a) const { return actionID == a.actionID; }
bool                ActionTypeData::operator <	(const ActionTypeData & a) const { return actionID < a.actionID; }

const PrerequisiteSet & ActionTypeData::getPrerequisites() 	const	{ return prerequisites; }
const PrerequisiteSet & ActionTypeData::getRecursivePrerequisites() 	const	{ return recursivePrerequisites; }
	
void ActionTypeData::setWhatBuildsActionID(const ActionID a, const bool wbib, const bool wbil)
{
	whatBuildsActionID = a;
	whatBuildsIsBuildingBool = wbib;
	whatBuildsIsLarvaBool = wbil;
}

void ActionTypeData::setRequiredAddon(bool requiresAddon, const ActionID & id)
{
    reqAddon = requiresAddon;
    reqAddonID = id;
}

////////////////////////////////////////////////////////////
// STATIC FUNCTIONS BELOW THIS POINT
////////////////////////////////////////////////////////////

//Vec< Vec<ActionTypeData, MAX_ACTION_TYPES>, Races::NUM_RACES> ActionTypeData::allActionTypeDataVec(Races::NUM_RACES);
//
//template <class ActionID> Vec< ActionID, Races::NUM_RACES >        ActionTypeData::numActionTypesVec(Races::NUM_RACES);
//template <class ActionID>Vec< ActionTypeData, Races::NUM_RACES >  ActionTypeData::workerActionTypes(Races::NUM_RACES);
//template <class ActionID>Vec< ActionTypeData, Races::NUM_RACES >  ActionTypeData::refineryActionTypes(Races::NUM_RACES);
//template <class ActionID>Vec< ActionTypeData, Races::NUM_RACES >  ActionTypeData::supplyProviderActionTypes(Races::NUM_RACES);
//template <class ActionID>Vec< ActionTypeData, Races::NUM_RACES >  ActionTypeData::resourceDepotActionTypes(Races::NUM_RACES);
//template <class DependencyGraph>Vec< DependencyGraph, Races::NUM_RACES > ActionTypeData::dependencyGraphs(Races::NUM_RACES);

std::vector< std::vector<ActionTypeData> >  ActionTypeData::allActionTypeDataVec(Races::NUM_RACES);
//std::vector<DependencyGraph>                ActionTypeData::dependencyGraphs(Races::NUM_RACES);

void ActionTypeData::Init()
{
    // add all the legal actions
    AddActions();
		
	CalculateWhatBuilds();

	// calculate the prerequisites of those actions
	AddPrerequisites();
		
	// calculates the dependency graph tree based on prerequisite graph
	//CalculateDependencyGraph();
}

void ActionTypeData::AddActions()
{
    allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Probe, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Pylon, allActionTypeDataVec[Races::Protoss].size()));		
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Assimilator, allActionTypeDataVec[Races::Protoss].size()));				
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Gateway, allActionTypeDataVec[Races::Protoss].size()));						
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Nexus, allActionTypeDataVec[Races::Protoss].size()));					
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Zealot, allActionTypeDataVec[Races::Protoss].size()));				
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Cybernetics_Core, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Dragoon, allActionTypeDataVec[Races::Protoss].size()));					
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UpgradeTypes::Singularity_Charge, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Forge, allActionTypeDataVec[Races::Protoss].size()));					
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Photon_Cannon, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_High_Templar, allActionTypeDataVec[Races::Protoss].size()));				
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Templar_Archives, allActionTypeDataVec[Races::Protoss].size()));			
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Robotics_Facility, allActionTypeDataVec[Races::Protoss].size()));		
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Robotics_Support_Bay, allActionTypeDataVec[Races::Protoss].size()));		
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Observatory, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Stargate, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Scout, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Arbiter, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Shield_Battery, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Dark_Templar, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Shuttle, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Reaver, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Observer, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Corsair, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Fleet_Beacon, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Carrier, allActionTypeDataVec[Races::Protoss].size()));
	allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UpgradeTypes::Leg_Enhancements, allActionTypeDataVec[Races::Protoss].size()));
    //allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(BWAPI::UnitTypes::Protoss_Archon, allActionTypeDataVec[Races::Protoss].size()));

    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_SCV, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Supply_Depot, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Command_Center, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Barracks, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Refinery, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Marine, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Academy, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::TechTypes::Stim_Packs, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Medic, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Factory, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Starport, allActionTypeDataVec[Races::Terran].size()));
	allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Wraith, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Machine_Shop, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::TechTypes::Tank_Siege_Mode, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::TechTypes::Cloaking_Field, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Control_Tower, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Battlecruiser, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Physics_Lab, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Science_Facility, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Science_Vessel, allActionTypeDataVec[Races::Terran].size()));
    allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(BWAPI::UnitTypes::Terran_Vulture, allActionTypeDataVec[Races::Terran].size()));

    allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Drone, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Overlord, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Hatchery, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Spawning_Pool, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Zergling, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Extractor, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Lair, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Hydralisk_Den, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Spire, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Hydralisk, allActionTypeDataVec[Races::Zerg].size()));
	allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Mutalisk, allActionTypeDataVec[Races::Zerg].size()));
    allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(BWAPI::UnitTypes::Zerg_Larva, allActionTypeDataVec[Races::Zerg].size()));

}

const RaceID ActionTypeData::GetRaceID(BWAPI::Race r)
{
    if (r == BWAPI::Races::Protoss)
    {
        return Races::Protoss;
    }
    else if (r == BWAPI::Races::Terran)
    {
        return Races::Terran;
    }
    else if (r == BWAPI::Races::Zerg)
    {
        return Races::Zerg;
    }
    else
    {
        return Races::None;
    }
}

const ActionID ActionTypeData::GetNumActionTypes(const RaceID race)
{
    return allActionTypeDataVec[race].size();
}

const ActionTypeData & ActionTypeData::GetActionTypeData(const RaceID raceID, const ActionID & id)
{
    return allActionTypeDataVec[raceID][id];
}



ActionID ActionTypeData::GetActionID(const BWAPI::UnitType & type) 
{
    const RaceID raceID = GetRaceID(type.getRace());
    for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
    {
        const ActionTypeData & data = GetActionTypeData(raceID, a);
        if (data.isUnit() && data.getUnitType() == type)
        {
            return data.getActionID();
        }
    }
   
	BOSS_ASSERT(false, "Could not find UnitType: %d %s", type.getID(), type.getName().c_str());
    return 0;
}

ActionID ActionTypeData::GetActionID(const BWAPI::TechType & type) 
{
    const RaceID raceID = GetRaceID(type.getRace());
    for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
    {
        const ActionTypeData & data = GetActionTypeData(raceID, a);
        if (data.isTech() && data.getTechType() == type)
        {
            return data.getActionID();
        }
    }
   
	BOSS_ASSERT(false, "Could not find TechType: %d %s", type.getID(), type.getName().c_str());
    return 0;
}

ActionID ActionTypeData::GetActionID(const BWAPI::UpgradeType & type) 
{
    const RaceID raceID = GetRaceID(type.getRace());
    for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
    {
        const ActionTypeData & data = GetActionTypeData(raceID, a);
        if (data.isUpgrade() && data.getUpgradeType() == type)
        {
            return data.getActionID();
        }
    }
   
	BOSS_ASSERT(false, "Could not find TechType: %d %s", type.getID(), type.getName().c_str());
    return 0;
}

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPI::UnitType & a)
{
    return GetActionTypeData(GetRaceID(a.getRace()), GetActionID(a));
}

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPI::TechType & a)
{
    return GetActionTypeData(GetRaceID(a.getRace()), GetActionID(a));
}

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPI::UpgradeType & a)
{
    return GetActionTypeData(GetRaceID(a.getRace()), GetActionID(a));
}


// adds all prerequisites
void ActionTypeData::AddPrerequisites()
{
    // set all prerequisites for all action types
	for (RaceID r(0); r < Races::NUM_RACES; ++r)
    {
        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
        {
		    allActionTypeDataVec[r][a].setPrerequisites(CalculatePrerequisites(allActionTypeDataVec[r][a]));
        }
    }

    // set required addon data
    for (RaceID r(0); r < Races::NUM_RACES; ++r)
    {
        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
        {
		    // set the prerequisite PrerequisiteSet based on the function below
            ActionTypeData & typeData = allActionTypeDataVec[r][a];
		    const PrerequisiteSet & pre = typeData.getPrerequisites();

            for (size_t p(0); p<pre.size(); ++p)
            {
                const ActionType & preActionType = pre.getActionType(p);

                // the addon has to be an addon of the building that construct the unit
                if (preActionType.isAddon() && (preActionType.whatBuildsAction() == typeData.whatBuildsActionID))
                {
                    typeData.setRequiredAddon(true, preActionType.ID());
                }
            }
        }
    }

    // set the recursive prerequisites (ie: the entire tech tree prereq for a given unit)
    for (RaceID r(0); r < Races::NUM_RACES; ++r)
    {
        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
        {
            PrerequisiteSet recursivePre;
            CalculateRecursivePrerequisites(recursivePre, allActionTypeDataVec[r][a]);
		    allActionTypeDataVec[r][a].setRecursivePrerequisites(recursivePre);
        }
    }
}
	
void ActionTypeData::CalculateWhatBuilds()
{
    for (RaceID r(0); r < Races::NUM_RACES; ++r)
    {
        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
        {
            ActionTypeData & actionTypeData = allActionTypeDataVec[r][a];
            if (actionTypeData.whatBuildsBWAPI() == BWAPI::UnitTypes::Zerg_Larva)
		    {
			    actionTypeData.setWhatBuildsActionID(GetActionID(BWAPI::UnitTypes::Zerg_Larva), false, true);
		    }
		    else
		    {
			    ActionID actionID = GetActionID(actionTypeData.whatBuildsBWAPI());
			    actionTypeData.setWhatBuildsActionID(actionID, actionTypeData.whatBuildsBWAPI().isBuilding(), false);
		    }	
        }
    }
}
	
// returns an PrerequisiteSet of prerequisites for a given action
PrerequisiteSet ActionTypeData::CalculatePrerequisites(const ActionTypeData & action)
{
	PrerequisiteSet pre;
    
	// if it's a UnitType
	if (action.getType() == ActionTypeData::UnitType)
	{
		std::map<BWAPI::UnitType, int> requiredUnits = action.getUnitType().requiredUnits();
		BWAPI::UnitType actionType = action.getUnitType();

		// if it's a protoss building that isn't a Nexus or Assimilator, we need a pylon (indirectly)
		if (actionType.getRace() == BWAPI::Races::Protoss && actionType.isBuilding() && !actionType.isResourceDepot() && 
			!(actionType == BWAPI::UnitTypes::Protoss_Pylon) && !(actionType == BWAPI::UnitTypes::Protoss_Assimilator))
		{
            pre.add(ActionTypes::Protoss_Pylon, 1);
		}

		// for each of the required UnitTypes
		for (std::map<BWAPI::UnitType, int>::iterator unitIt = requiredUnits.begin(); unitIt != requiredUnits.end(); unitIt++)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", unitIt->first.getName().c_str());
	
			BWAPI::UnitType type = unitIt->first;
            int count = unitIt->second;

			// add the action to the PrerequisiteSet if it is not a larva
			if (type != BWAPI::UnitTypes::Zerg_Larva)
			{
				//printf("\t\tAdding %s\n", type.getName().c_str());
				pre.add(ActionType(action.getRaceID(), GetActionID(type)), count);
			}
		}

		// if there is a TechType required
		if (action.getUnitType().requiredTech() != BWAPI::TechTypes::None)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", action.getUnitType().requiredTech().getName().c_str());
            BWAPI::TechType required = action.getUnitType().requiredTech();
			// add it to the PrerequisiteSet
			pre.add(ActionType(action.getRaceID(), GetActionID(required)), 1);
		}
	}

	// if it's a TechType
	if (action.getType() == ActionTypeData::TechType)
	{
		if (action.getTechType().whatResearches() != BWAPI::UnitTypes::None)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", action.getTechType().whatResearches().getName().c_str());

			// add what researches it
			pre.add(ActionType(action.getRaceID(), GetActionID(action.getTechType().whatResearches())), 1);
		}
	}

	// if it's an UpgradeType
	if (action.getType() == ActionTypeData::UpgradeType)
	{
		if (action.getUpgradeType().whatUpgrades() != BWAPI::UnitTypes::None)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", action.getUpgradeType().whatUpgrades().getName().c_str());

			// add what upgrades it
			pre.add(ActionType(action.getRaceID(), GetActionID(action.getUpgradeType().whatUpgrades())), 1);
		}
	}


	//printf("Finish Prerequisites\n");
	return pre;
}

// calculate the recursive prerequisites for an Action, storing them in allPre
// assumes that prerequisites have already been calculated
void ActionTypeData::CalculateRecursivePrerequisites(PrerequisiteSet & allPre, const ActionTypeData & action)
{
	PrerequisiteSet pre = action.getPrerequisites();

    if (action.gasPrice() > 0)
    {
        if (action.raceID == Races::Protoss)
        {
            pre.add(ActionTypes::Protoss_Assimilator);
        }
        else if (action.raceID == Races::Terran)
        {
            pre.add(ActionTypes::Terran_Refinery);
        }
        if (action.raceID == Races::Zerg)
        {
            pre.add(ActionTypes::Zerg_Extractor);
        }
    }

	for (size_t a(0); a < pre.size(); ++a)
    {
        const ActionType & actionType = pre.getActionType(a);

        if (!allPre.contains(actionType))
        {
            allPre.add(actionType);
            CalculateRecursivePrerequisites(allPre, GetActionTypeData(actionType.getRace(), actionType.ID()));
        }
    }
}
