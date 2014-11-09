#include "ActionTypeData.h"
#include "ActionType.h"

using namespace BOSS;

// default constructor, should never be called
ActionTypeData::ActionTypeData() 
    : type(Error) 
{ 
   
}

// UnitType constructor
ActionTypeData::ActionTypeData(BWAPIDATA::UnitType t, const ActionID id) 
	: type                      (UnitType)
	, unit                      (t)
    , raceID                    (GetRaceID(t.getRace()))
	, actionID                  (id)
	, mineralPriceVal           (t.mineralPrice() * Constants::RESOURCE_SCALE)
	, gasPriceVal               (t.gasPrice() * Constants::RESOURCE_SCALE)
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
    , morphed                   (false)
    , reqAddon                  (false)
    , reqAddonID                (0)
{
	if (t == BWAPIDATA::UnitTypes::Zerg_Zergling || t == BWAPIDATA::UnitTypes::Zerg_Scourge)
	{
		numberProduced = 2;
	}

    if (t == BWAPIDATA::UnitTypes::Zerg_Lair ||
        t == BWAPIDATA::UnitTypes::Zerg_Hive ||
        t == BWAPIDATA::UnitTypes::Zerg_Greater_Spire ||
        t == BWAPIDATA::UnitTypes::Zerg_Lurker ||
        t == BWAPIDATA::UnitTypes::Zerg_Guardian ||
        t == BWAPIDATA::UnitTypes::Zerg_Sunken_Colony ||
        t == BWAPIDATA::UnitTypes::Zerg_Spore_Colony)
    {
        morphed = true;
    }
}

// UpgradeType action
ActionTypeData::ActionTypeData(BWAPIDATA::UpgradeType t, const ActionID id) 
	: type(UpgradeType)
	, upgrade(t)
	, actionID(id)
    , raceID(GetRaceID(t.getRace()))
	, mineralPriceVal(t.mineralPrice() * Constants::RESOURCE_SCALE)
	, gasPriceVal(t.gasPrice() * Constants::RESOURCE_SCALE)
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
    , morphed(false)
{
}

// TechType action
ActionTypeData::ActionTypeData(BWAPIDATA::TechType t, const ActionID id)
	: type(TechType)
	, tech(t)
	, actionID(id)
    , raceID(GetRaceID(t.getRace()))
	, mineralPriceVal(t.mineralPrice() * Constants::RESOURCE_SCALE)
	, gasPriceVal(t.gasPrice() * Constants::RESOURCE_SCALE)
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
    , morphed(false)
{}

RaceID              ActionTypeData::getRaceID()             const   { return raceID; }
ActionID            ActionTypeData::getActionID()           const   { return actionID; }

BWAPIDATA::UnitType		ActionTypeData::getUnitType()			const	{ return unit; }
BWAPIDATA::UnitType		ActionTypeData::whatBuildsBWAPI() 		const	{ return whatBuildsUnitType; }
BWAPIDATA::UpgradeType	ActionTypeData::getUpgradeType()		const	{ return upgrade; }
BWAPIDATA::TechType		ActionTypeData::getTechType()			const	{ return tech; }
	
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
bool                ActionTypeData::isMorphed()             const   { return morphed; }
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
    std::string protoss("Protoss");
    std::string terran("Terran");
    std::string zerg("Zerg");
    std::set<BWAPIDATA::UnitType> unitTypesSet = BWAPIDATA::UnitTypes::allUnitTypes();
    std::vector<BWAPIDATA::UnitType> unitTypes;
    std::copy(unitTypesSet.begin(), unitTypesSet.end(), std::back_inserter(unitTypes));

    for (size_t i(0); i < unitTypes.size()-1; ++i)
    {
        for (size_t j(i+1); j < unitTypes.size(); ++j)
        {
            int costi = unitTypes[i].mineralPrice() + 2*unitTypes[i].gasPrice();
            int costj = unitTypes[j].mineralPrice() + 2*unitTypes[j].gasPrice();

            if (costi > costj)
            {
                std::swap(unitTypes[i], unitTypes[j]);
            }
        }
    }

    for (size_t i(0); i < unitTypes.size(); ++i)
    {
        const BWAPIDATA::UnitType & type = unitTypes[i];

        // blacklist temporary unit types which aren't buildable
        if (type == BWAPIDATA::UnitTypes::Zerg_Egg ||
            type == BWAPIDATA::UnitTypes::Zerg_Lurker_Egg ||
            type == BWAPIDATA::UnitTypes::Zerg_Cocoon ||
            type == BWAPIDATA::UnitTypes::Zerg_Infested_Terran ||
            type == BWAPIDATA::UnitTypes::Zerg_Infested_Command_Center ||
            type == BWAPIDATA::UnitTypes::Zerg_Broodling ||
            type == BWAPIDATA::UnitTypes::Protoss_Interceptor ||
            type == BWAPIDATA::UnitTypes::Protoss_Scarab ||
            type == BWAPIDATA::UnitTypes::Terran_Civilian ||
            type == BWAPIDATA::UnitTypes::Terran_Nuclear_Missile ||
            type == BWAPIDATA::UnitTypes::Terran_Vulture_Spider_Mine)
        {
            continue;
        }

        if (type.getName().compare(0, protoss.length(), protoss) == 0)
        {
            allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Protoss].size()));
        }
        else if (type.getName().compare(0, terran.length(), terran) == 0)
        {
            allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Terran].size()));
        }
        else if (type.getName().compare(0, zerg.length(), zerg) == 0)
        {
            allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Zerg].size()));
        }
    }

    const std::set<BWAPIDATA::TechType> & techTypes = BWAPIDATA::TechTypes::allTechTypes();
    std::set<BWAPIDATA::TechType>::const_iterator tit;
    for (tit = techTypes.begin(); tit != techTypes.end(); tit++)
    {
        const BWAPIDATA::TechType & type = *tit;

        // blacklisted tech types we cannot create
        if (type.whatResearches() == BWAPIDATA::UnitTypes::None)
        {
            continue;
        }

        if (type.getRace() == BWAPIDATA::Races::Protoss)
        {
            allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Protoss].size()));
        }
        else if (type.getRace() == BWAPIDATA::Races::Terran)
        {
            allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Terran].size()));
        }
        else if (type.getRace() == BWAPIDATA::Races::Zerg)
        {
            allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Zerg].size()));
        }
    }

    const std::set<BWAPIDATA::UpgradeType> & upgradeTypes = BWAPIDATA::UpgradeTypes::allUpgradeTypes();
    std::set<BWAPIDATA::UpgradeType>::const_iterator uit;
    for (uit = upgradeTypes.begin(); uit != upgradeTypes.end(); uit++)
    {
        const BWAPIDATA::UpgradeType & type = *uit;

        // blacklisted tech types we cannot create
        if (type.whatUpgrades() == BWAPIDATA::UnitTypes::None)
        {
            continue;
        }

        if (type.getRace() == BWAPIDATA::Races::Protoss)
        {
            allActionTypeDataVec[Races::Protoss].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Protoss].size()));
        }
        else if (type.getRace() == BWAPIDATA::Races::Terran)
        {
            allActionTypeDataVec[Races::Terran].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Terran].size()));
        }
        else if (type.getRace() == BWAPIDATA::Races::Zerg)
        {
            allActionTypeDataVec[Races::Zerg].push_back(ActionTypeData(type, allActionTypeDataVec[Races::Zerg].size()));
        }
    }
}

const RaceID ActionTypeData::GetRaceID(BWAPIDATA::Race r)
{
    if (r == BWAPIDATA::Races::Protoss)
    {
        return Races::Protoss;
    }
    else if (r == BWAPIDATA::Races::Terran)
    {
        return Races::Terran;
    }
    else if (r == BWAPIDATA::Races::Zerg)
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



ActionID ActionTypeData::GetActionID(const BWAPIDATA::UnitType & type) 
{
    const RaceID raceID = GetRaceID(type.getRace());
    BOSS_ASSERT(raceID < Races::NUM_RACES, "Race ID invalid: %d %s", (int)raceID, type.getName().c_str());


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

ActionID ActionTypeData::GetActionID(const BWAPIDATA::TechType & type) 
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

ActionID ActionTypeData::GetActionID(const BWAPIDATA::UpgradeType & type) 
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

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPIDATA::UnitType & a)
{
    return GetActionTypeData(GetRaceID(a.getRace()), GetActionID(a));
}

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPIDATA::TechType & a)
{
    return GetActionTypeData(GetRaceID(a.getRace()), GetActionID(a));
}

ActionTypeData ActionTypeData::GetActionTypeData(const BWAPIDATA::UpgradeType & a)
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
            if (actionTypeData.whatBuildsBWAPI() == BWAPIDATA::UnitTypes::Zerg_Larva)
		    {
			    actionTypeData.setWhatBuildsActionID(GetActionID(BWAPIDATA::UnitTypes::Zerg_Larva), false, true);
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
		std::map<BWAPIDATA::UnitType, int> requiredUnits = action.getUnitType().requiredUnits();
		BWAPIDATA::UnitType actionType = action.getUnitType();

		// if it's a protoss building that isn't a Nexus or Assimilator, we need a pylon (indirectly)
		if (actionType.getRace() == BWAPIDATA::Races::Protoss && actionType.isBuilding() && !actionType.isResourceDepot() && 
			!(actionType == BWAPIDATA::UnitTypes::Protoss_Pylon) && !(actionType == BWAPIDATA::UnitTypes::Protoss_Assimilator))
		{
            pre.add(ActionType(Races::Protoss, GetActionID(BWAPIDATA::UnitTypes::Protoss_Pylon)), 1);
		}

		// for each of the required UnitTypes
		for (std::map<BWAPIDATA::UnitType, int>::iterator unitIt = requiredUnits.begin(); unitIt != requiredUnits.end(); unitIt++)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", unitIt->first.getName().c_str());
	
			BWAPIDATA::UnitType type = unitIt->first;
            int count = unitIt->second;

			// add the action to the PrerequisiteSet if it is not a larva
			if (type != BWAPIDATA::UnitTypes::Zerg_Larva)
			{
				//printf("\t\tAdding %s\n", type.getName().c_str());
				pre.add(ActionType(action.getRaceID(), GetActionID(type)), count);
			}
		}

		// if there is a TechType required
		if (action.getUnitType().requiredTech() != BWAPIDATA::TechTypes::None)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", action.getUnitType().requiredTech().getName().c_str());
            BWAPIDATA::TechType required = action.getUnitType().requiredTech();
			// add it to the PrerequisiteSet
			pre.add(ActionType(action.getRaceID(), GetActionID(required)), 1);
		}
	}

	// if it's a TechType
	if (action.getType() == ActionTypeData::TechType)
	{
		if (action.getTechType().whatResearches() != BWAPIDATA::UnitTypes::None)
		{
			//if (DEBUG_StarcraftData) printf("\tPRE: %s\n", action.getTechType().whatResearches().getName().c_str());

			// add what researches it
			pre.add(ActionType(action.getRaceID(), GetActionID(action.getTechType().whatResearches())), 1);
		}
	}

	// if it's an UpgradeType
	if (action.getType() == ActionTypeData::UpgradeType)
	{
		if (action.getUpgradeType().whatUpgrades() != BWAPIDATA::UnitTypes::None)
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
            pre.add(ActionType(Races::Protoss, GetActionID(BWAPIDATA::UnitTypes::Protoss_Assimilator)));
        }
        else if (action.raceID == Races::Terran)
        {
            pre.add(ActionType(Races::Terran, GetActionID(BWAPIDATA::UnitTypes::Terran_Refinery)));
        }
        if (action.raceID == Races::Zerg)
        {
            pre.add(ActionType(Races::Zerg, GetActionID(BWAPIDATA::UnitTypes::Zerg_Extractor)));
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
