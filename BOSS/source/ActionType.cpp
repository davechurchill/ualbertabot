#include "ActionType.h"

#include "ActionTypeData.h"
#include "ActionSet.h"

using namespace BOSS;

ActionType::ActionType()
: _race(Races::None)
, _id(0)
{

}

ActionType::ActionType(const RaceID & race, const ActionID & id)
: _race(race)
, _id(id)
{

}

ActionType::ActionType(const ActionType & type)
: _race(type._race)
, _id(type._id)
{

}

ActionType::ActionType(const BWAPI::UnitType & type)
    : _race(ActionTypeData::GetRaceID(type.getRace()))
    , _id(ActionTypeData::GetActionID(type))
{

}

ActionType::ActionType(const BWAPI::UpgradeType & type)
    : _race(ActionTypeData::GetRaceID(type.getRace()))
    , _id(ActionTypeData::GetActionID(type))
{

}

ActionType::ActionType(const BWAPI::TechType & type)
    : _race(ActionTypeData::GetRaceID(type.getRace()))
    , _id(ActionTypeData::GetActionID(type))
{

}

ActionType & ActionType::operator = (const ActionType & rhs)
{
    if (this != &rhs)
    {
        new (this) ActionType(rhs);
    }

    return *this;
}   

const ActionID              ActionType::ID()                    const { return _id; }
const RaceID                ActionType::getRace()               const { return _race; }

BWAPI::UnitType             ActionType::getUnitType()           const { return ActionTypeData::GetActionTypeData(_race, _id).getUnitType(); }
BWAPI::UpgradeType          ActionType::getUpgradeType()        const { return ActionTypeData::GetActionTypeData(_race, _id).getUpgradeType(); }
BWAPI::TechType             ActionType::getTechType()           const { return ActionTypeData::GetActionTypeData(_race, _id).getTechType(); }

BWAPI::UnitType             ActionType::whatBuildsBWAPI()       const { return ActionTypeData::GetActionTypeData(_race, _id).whatBuildsBWAPI(); }
ActionType		            ActionType::whatBuildsActionType()  const { return ActionTypes::GetActionType(_race, whatBuildsAction()); }
ActionID                    ActionType::whatBuildsAction()      const { return ActionTypeData::GetActionTypeData(_race, _id).whatBuildsAction(); }	

const PrerequisiteSet &     ActionType::getPrerequisites()      const { return ActionTypeData::GetActionTypeData(_race, _id).getPrerequisites(); }
const PrerequisiteSet &     ActionType::getRecursivePrerequisites()      const { return ActionTypeData::GetActionTypeData(_race, _id).getRecursivePrerequisites(); }
int                         ActionType::getType()               const { return ActionTypeData::GetActionTypeData(_race, _id).getType(); }
	
const std::string &         ActionType::getName()               const { return ActionTypeData::GetActionTypeData(_race, _id).getName(); }
const std::string &         ActionType::getShortName()          const { return ActionTypeData::GetActionTypeData(_race, _id).getShortName(); }
const std::string &         ActionType::getMetaName()           const { return ActionTypeData::GetActionTypeData(_race, _id).getMetaName(); }
	
FrameCountType              ActionType::buildTime()             const { return ActionTypeData::GetActionTypeData(_race, _id).buildTime(); }
ResourceCountType           ActionType::mineralPrice()          const { return ActionTypeData::GetActionTypeData(_race, _id).mineralPrice(); }
ResourceCountType           ActionType::mineralPriceScaled()    const { return ActionTypeData::GetActionTypeData(_race, _id).mineralPriceScaled(); }
ResourceCountType           ActionType::gasPrice()              const { return ActionTypeData::GetActionTypeData(_race, _id).gasPrice(); }
ResourceCountType           ActionType::gasPriceScaled()        const { return ActionTypeData::GetActionTypeData(_race, _id).gasPriceScaled(); }
SupplyCountType             ActionType::supplyRequired()        const { return ActionTypeData::GetActionTypeData(_race, _id).supplyRequired(); }
SupplyCountType             ActionType::supplyProvided()        const { return ActionTypeData::GetActionTypeData(_race, _id).supplyProvided(); }
UnitCountType               ActionType::numProduced()           const { return ActionTypeData::GetActionTypeData(_race, _id).numProduced(); }

bool                        ActionType::isAddon()               const { return ActionTypeData::GetActionTypeData(_race, _id).isAddon(); }
bool                        ActionType::isRefinery()            const { return ActionTypeData::GetActionTypeData(_race, _id).isRefinery(); }
bool                        ActionType::isWorker()              const { return ActionTypeData::GetActionTypeData(_race, _id).isWorker(); }
bool                        ActionType::isBuilding()            const { return ActionTypeData::GetActionTypeData(_race, _id).isBuilding(); }
bool                        ActionType::isResourceDepot()       const { return ActionTypeData::GetActionTypeData(_race, _id).isResourceDepot(); }
bool                        ActionType::isSupplyProvider()      const { return ActionTypeData::GetActionTypeData(_race, _id).isSupplyProvider(); }
bool                        ActionType::isUnit()                const { return ActionTypeData::GetActionTypeData(_race, _id).isUnit(); }
bool                        ActionType::isTech()                const { return ActionTypeData::GetActionTypeData(_race, _id).isTech(); }
bool                        ActionType::isUpgrade()             const { return ActionTypeData::GetActionTypeData(_race, _id).isUpgrade(); }
bool                        ActionType::whatBuildsIsBuilding()  const { return ActionTypeData::GetActionTypeData(_race, _id).whatBuildsIsBuilding(); }
bool                        ActionType::whatBuildsIsLarva()     const { return ActionTypeData::GetActionTypeData(_race, _id).whatBuildsIsLarva(); }
bool                        ActionType::canProduce()            const { return ActionTypeData::GetActionTypeData(_race, _id).canProduce(); }
bool                        ActionType::requiresAddon()         const { return ActionTypeData::GetActionTypeData(_race, _id).requiresAddon(); }
bool                        ActionType::isMorphed()             const { return ActionTypeData::GetActionTypeData(_race, _id).isMorphed(); }

bool ActionType::canBuild(const ActionType & t) const 
{ 
    if (t.getRace() != getRace())
    {
        return false;
    }

    static const ActionType & Hatchery      = ActionTypes::GetActionType("Zerg_Hatchery");
    static const ActionType & Lair          = ActionTypes::GetActionType("Zerg_Lair");
    static const ActionType & Hive          = ActionTypes::GetActionType("Zerg_Hive");
    static const ActionType & Spire         = ActionTypes::GetActionType("Zerg_Spire");
    static const ActionType & GreaterSpire  = ActionTypes::GetActionType("Zerg_Greater_Spire");

    const ActionType & whatBuilds = t.whatBuildsActionType();

    if (whatBuilds == *this)
    {
        return true;
    }
    
    // if this is a zerg unit which is not morphed
    // then an upgraded version of the building can construct it
    // morphed units need the *exact* unit to morph from which is covered above
    if ((t.getRace() == Races::Zerg) && !t.isMorphed())
    {
        if (whatBuilds == Hatchery)
        {
            return (*this == Lair || *this == Hive);
        }
        else if (whatBuilds == Lair)
        {
            return (*this == Hive);
        }
        else if (whatBuilds == Spire)
        {
            return (*this == GreaterSpire);
        }
    }
    
    return false;
}

ActionType                  ActionType::requiredAddonType()     const { return ActionTypes::GetActionType(_race, ActionTypeData::GetActionTypeData(_race, _id).requiredAddonID()); }

const bool ActionType::operator == (const ActionType & rhs)     const { return _race == rhs._race && _id == rhs._id; }
const bool ActionType::operator != (const ActionType & rhs)     const { return _race != rhs._race || _id != rhs._id; }
const bool ActionType::operator <  (const ActionType & rhs)     const { return _id < rhs._id; }

namespace BOSS
{
namespace ActionTypes
{
    std::vector< std::vector<ActionType> > allActionTypes;
    std::map<std::string, ActionType> nameMap;
    std::vector<ActionType>  workerActionTypes;
    std::vector<ActionType>  refineryActionTypes;
    std::vector<ActionType>  supplyProviderActionTypes;
    std::vector<ActionType>  resourceDepotActionTypes;
    size_t numActionTypes[Races::NUM_RACES] = {0, 0, 0};

    void init()
    {
        for (RaceID r(0); r < Races::NUM_RACES; ++r)
        {
            allActionTypes.push_back(std::vector<ActionType>());
            for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
            {
                ActionType type(r, a);
                allActionTypes[r].push_back(type);

                // add the name with spaces
                nameMap[type.getName()] = type;

                // add the name with underscores
                std::string name = type.getName();
                for (size_t i(0); i<name.size(); ++i)
	            {
		            if (name[i] == ' ')
		            {
			            name[i] = '_';
		            }
	            }
                nameMap[name] = type;
            }
        }

        workerActionTypes.push_back(ActionTypes::GetActionType("Protoss_Probe"));
        refineryActionTypes.push_back(ActionTypes::GetActionType("Protoss_Assimilator"));
        supplyProviderActionTypes.push_back(ActionTypes::GetActionType("Protoss_Pylon"));
        resourceDepotActionTypes.push_back(ActionTypes::GetActionType("Protoss_Nexus"));

        workerActionTypes.push_back(ActionTypes::GetActionType("Terran_SCV"));
        refineryActionTypes.push_back(ActionTypes::GetActionType("Terran_Refinery"));
        supplyProviderActionTypes.push_back(ActionTypes::GetActionType("Terran_Supply_Depot"));
        resourceDepotActionTypes.push_back(ActionTypes::GetActionType("Terran_Command_Center"));

        workerActionTypes.push_back(ActionTypes::GetActionType("Zerg_Drone"));
        refineryActionTypes.push_back(ActionTypes::GetActionType("Zerg_Extractor"));
        supplyProviderActionTypes.push_back(ActionTypes::GetActionType("Zerg_Overlord"));
        resourceDepotActionTypes.push_back(ActionTypes::GetActionType("Zerg_Hatchery"));
    }

    const ActionType & GetWorker(const RaceID raceID)
    {
        return workerActionTypes[raceID];
    }

    const ActionType & GetSupplyProvider(const RaceID raceID)
    {
        return supplyProviderActionTypes[raceID];
    }

    const ActionType & GetRefinery(const RaceID raceID)
    {
        return refineryActionTypes[raceID];
    }

    const ActionType & GetResourceDepot(const RaceID raceID)
    {
        return resourceDepotActionTypes[raceID];
    }

    //const ActionType & GetActionType(const std::string & name)
    //{
    //    std::map<
    //    auto it = nameMap.find(name);

    //    BOSS_ASSERT(it != nameMap.end(), "ActionType::getActionType() error: ActionType name not found: %s", name.c_str());

    //    return it->second;
    //}

    const ActionType & GetActionType(const RaceID & race, const ActionID & id)
    {
        BOSS_ASSERT(race < Races::NUM_RACES && id < ActionTypes::GetAllActionTypes(race).size(), "Race / Action does not exist");

        return allActionTypes[race][id];
    }

    const ActionType & GetActionType(const std::string & name)
    {
        BOSS_ASSERT(TypeExists(name), "ActionType name not found: %s", name.c_str());

        return nameMap[name];
    }

    const bool TypeExists(const std::string & name) 
    {
        return nameMap.find(name) != nameMap.end();
    }

    const std::vector<ActionType> & GetAllActionTypes(const RaceID race)
    {
        BOSS_ASSERT(race < Races::NUM_RACES, "Race is not valid: %d", race);

        return allActionTypes[race];
    }

    const bool TypeExists(const BWAPI::UnitType & type) 
    {
        const RaceID raceID = ActionTypeData::GetRaceID(type.getRace());

        if (raceID >= Races::NUM_RACES)
        {
            return false;
        }

        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
        {
            const ActionType & data = GetActionType(raceID, a);
            if (data.isUnit() && data.getUnitType() == type)
            {
                return true;
            }
        }
   
	    return false;
    }

    const bool TypeExists(const BWAPI::UpgradeType & type) 
    {
        const RaceID raceID = ActionTypeData::GetRaceID(type.getRace());

        if (raceID >= Races::NUM_RACES)
        {
            return false;
        }

        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
        {
            const ActionType & data = GetActionType(raceID, a);
            if (data.isUpgrade() && data.getUpgradeType() == type)
            {
                return true;
            }
        }
   
	    return false;
    }

    const bool TypeExists(const BWAPI::TechType & type) 
    {
        const RaceID raceID = ActionTypeData::GetRaceID(type.getRace());

        if (raceID >= Races::NUM_RACES)
        {
            return false;
        }
        
        for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(raceID); ++a)
        {
            const ActionType & data = GetActionType(raceID, a);
            if (data.isTech() && data.getTechType() == type)
            {
                return true;
            }
        }
   
	    return false;
    }

    ActionType None(Races::None, 0);

 //   ActionType Protoss_Probe                (Races::Protoss, numActionTypes[Races::Protoss]++);						
	//ActionType Protoss_Pylon                (Races::Protoss, numActionTypes[Races::Protoss]++);		
	//ActionType Protoss_Assimilator          (Races::Protoss, numActionTypes[Races::Protoss]++);						
	//ActionType Protoss_Gateway              (Races::Protoss, numActionTypes[Races::Protoss]++);				
	//ActionType Protoss_Nexus                (Races::Protoss, numActionTypes[Races::Protoss]++);					
	//ActionType Protoss_Zealot               (Races::Protoss, numActionTypes[Races::Protoss]++);				
	//ActionType Protoss_Cybernetics_Core     (Races::Protoss, numActionTypes[Races::Protoss]++);			
	//ActionType Protoss_Dragoon              (Races::Protoss, numActionTypes[Races::Protoss]++);					
	//ActionType Protoss_Singularity_Charge   (Races::Protoss, numActionTypes[Races::Protoss]++);			
	//ActionType Protoss_Forge                (Races::Protoss, numActionTypes[Races::Protoss]++);					
	//ActionType Protoss_Photon_Cannon        (Races::Protoss, numActionTypes[Races::Protoss]++);			
	//ActionType Protoss_High_Templar         (Races::Protoss, numActionTypes[Races::Protoss]++);				
	//ActionType Protoss_Citadel_of_Adun      (Races::Protoss, numActionTypes[Races::Protoss]++);			
	//ActionType Protoss_Templar_Archives     (Races::Protoss, numActionTypes[Races::Protoss]++);			
	//ActionType Protoss_Robotics_Facility    (Races::Protoss, numActionTypes[Races::Protoss]++);		
	//ActionType Protoss_Robotics_Support_Bay (Races::Protoss, numActionTypes[Races::Protoss]++);		
	//ActionType Protoss_Observatory          (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Stargate             (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Scout                (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Arbiter_Tribunal     (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Arbiter              (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Shield_Battery       (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Dark_Templar         (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Shuttle              (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Reaver               (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Observer             (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Corsair              (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Fleet_Beacon         (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Carrier              (Races::Protoss, numActionTypes[Races::Protoss]++);
	//ActionType Protoss_Leg_Enhancements     (Races::Protoss, numActionTypes[Races::Protoss]++);
 //   //ActionType Protoss_Archon(Races::Protoss, 31);

 //   ActionType Terran_SCV(Races::Terran, 0);
	//ActionType Terran_Supply_Depot(Races::Terran, 1);
	//ActionType Terran_Command_Center(Races::Terran, 2);
	//ActionType Terran_Barracks(Races::Terran, 3);
	//ActionType Terran_Refinery(Races::Terran, 4);
	//ActionType Terran_Marine(Races::Terran, 5);
	//ActionType Terran_Academy(Races::Terran, 6);
	//ActionType Terran_Stim_Packs(Races::Terran, 7);
	//ActionType Terran_Medic(Races::Terran, 8);
	//ActionType Terran_Factory(Races::Terran, 9);
	//ActionType Terran_Starport(Races::Terran, 10);
	//ActionType Terran_Wraith(Races::Terran, 11);
 //   ActionType Terran_Siege_Tank(Races::Terran, 12);
 //   ActionType Terran_Machine_Shop(Races::Terran, 13);
 //   ActionType Terran_Tank_Siege_Mode(Races::Terran, 14);
 //   ActionType Terran_Cloaking_Field(Races::Terran, 15);
 //   ActionType Terran_Control_Tower(Races::Terran, 16);
 //   ActionType Terran_Battlecruiser(Races::Terran, 17);
 //   ActionType Terran_Physics_Lab(Races::Terran, 18);
 //   ActionType Terran_Science_Facility(Races::Terran, 19);
 //   ActionType Terran_Science_Vessel(Races::Terran, 20);
 //   ActionType Terran_Vulture(Races::Terran, 21);
 //   ActionType Terran_Goliath(Races::Terran, 22);
 //   ActionType Terran_Firebat(Races::Terran, 23);
 //   ActionType Terran_Armory(Races::Terran, 24);
 //   ActionType Terran_Ghost(Races::Terran, 25);
 //   ActionType Terran_Covert_Ops(Races::Terran, 26);
 //   ActionType Terran_Comsat_Station(Races::Terran, 27);
 //   ActionType Terran_Bunker(Races::Terran, 28);
 //   ActionType Terran_Valkyrie(Races::Terran, 29);
 //   
 //   ActionType Zerg_Drone(Races::Zerg, 0);
	//ActionType Zerg_Overlord(Races::Zerg, 1);
	//ActionType Zerg_Hatchery(Races::Zerg, 2);
	//ActionType Zerg_Spawning_Pool(Races::Zerg, 3);
	//ActionType Zerg_Zergling(Races::Zerg, 4);
	//ActionType Zerg_Extractor(Races::Zerg, 5);
	//ActionType Zerg_Lair(Races::Zerg, 6);
	//ActionType Zerg_Hydralisk_Den(Races::Zerg, 7);
	//ActionType Zerg_Spire(Races::Zerg, 8);
	//ActionType Zerg_Hydralisk(Races::Zerg, 9);
	//ActionType Zerg_Mutalisk(Races::Zerg, 10);
 //   ActionType Zerg_Larva(Races::Zerg, 11);
 //   ActionType Zerg_Hive(Races::Zerg, 12);
 //   ActionType Zerg_Greater_Spire(Races::Zerg, 13);
 //   ActionType Zerg_Guardian(Races::Zerg, 14);
 //   ActionType Zerg_Defiler(Races::Zerg, 15);
 //   ActionType Zerg_Defiler_Mound(Races::Zerg, 16);
 //   ActionType Zerg_Devourer(Races::Zerg, 17);
 //   ActionType Zerg_Scourge(Races::Zerg, 18);
 //   ActionType Zerg_Queen(Races::Zerg, 19);
 //   ActionType Zerg_Queens_Nest(Races::Zerg, 20);
 //   ActionType Zerg_Ultralisk(Races::Zerg, 21);
 //   ActionType Zerg_Ultralisk_Cavern(Races::Zerg, 22);
 //   ActionType Zerg_Creep_Colony(Races::Zerg, 23);
 //   ActionType Zerg_Sunken_Colony(Races::Zerg, 24);
 //   ActionType Zerg_Spore_Colony(Races::Zerg, 25);
 //   ActionType Zerg_Evolution_Chamber(Races::Zerg, 26);
 //   ActionType Zerg_Lurker(Races::Zerg, 27);
 //   ActionType Zerg_Lurker_Aspect(Races::Zerg, 28);
}
}