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
	
std::string                 ActionType::getName()               const { return ActionTypeData::GetActionTypeData(_race, _id).getName(); }
std::string                 ActionType::getMetaName()           const { return ActionTypeData::GetActionTypeData(_race, _id).getMetaName(); }
	
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

ActionType                  ActionType::requiredAddonType()     const { return ActionTypes::GetActionType(_race, ActionTypeData::GetActionTypeData(_race, _id).requiredAddonID()); }

const bool ActionType::operator == (const ActionType & rhs)     const { return _race == rhs._race && _id == rhs._id; }
const bool ActionType::operator != (const ActionType & rhs)     const { return _race != rhs._race || _id != rhs._id; }
const bool ActionType::operator <  (const ActionType & rhs)     const { return _id < rhs._id; }

namespace BOSS
{
namespace ActionTypes
{
    std::vector<ActionSet> allActionTypes;
    std::map<std::string, ActionType> nameMap;
    std::vector<ActionType>  workerActionTypes;
    std::vector<ActionType>  refineryActionTypes;
    std::vector<ActionType>  supplyProviderActionTypes;
    std::vector<ActionType>  resourceDepotActionTypes;

    void init()
    {
        for (RaceID r(0); r < Races::NUM_RACES; ++r)
        {
            allActionTypes.push_back(ActionSet());
            for (ActionID a(0); a < ActionTypeData::GetNumActionTypes(r); ++a)
            {
                ActionType type(r, a);
                allActionTypes[r].add(type);
                nameMap[type.getName()] = type;
            }
        }

        workerActionTypes.push_back(ActionTypes::Protoss_Probe);
        refineryActionTypes.push_back(ActionTypes::Protoss_Assimilator);
        supplyProviderActionTypes.push_back(ActionTypes::Protoss_Pylon);
        resourceDepotActionTypes.push_back(ActionTypes::Protoss_Nexus);

        workerActionTypes.push_back(ActionTypes::Terran_SCV);
        refineryActionTypes.push_back(ActionTypes::Terran_Refinery);
        supplyProviderActionTypes.push_back(ActionTypes::Terran_Supply_Depot);
        resourceDepotActionTypes.push_back(ActionTypes::Terran_Command_Center);

        workerActionTypes.push_back(ActionTypes::Zerg_Drone);
        refineryActionTypes.push_back(ActionTypes::Zerg_Extractor);
        supplyProviderActionTypes.push_back(ActionTypes::Zerg_Overlord);
        resourceDepotActionTypes.push_back(ActionTypes::Zerg_Hatchery);
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

    const ActionSet & GetAllActionTypes(const RaceID race)
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

    ActionType Protoss_Probe(Races::Protoss, 0);						
	ActionType Protoss_Pylon(Races::Protoss, 1);		
	ActionType Protoss_Assimilator(Races::Protoss, 2);						
	ActionType Protoss_Gateway(Races::Protoss, 3);				
	ActionType Protoss_Nexus(Races::Protoss, 4);					
	ActionType Protoss_Zealot(Races::Protoss, 5);				
	ActionType Protoss_Cybernetics_Core(Races::Protoss, 6);			
	ActionType Protoss_Dragoon(Races::Protoss, 7);					
	ActionType Protoss_Singularity_Charge(Races::Protoss, 8);			
	ActionType Protoss_Forge(Races::Protoss, 9);					
	ActionType Protoss_Photon_Cannon(Races::Protoss, 10);			
	ActionType Protoss_High_Templar(Races::Protoss, 11);				
	ActionType Protoss_Citadel_of_Adun(Races::Protoss, 12);			
	ActionType Protoss_Templar_Archives(Races::Protoss, 13);			
	ActionType Protoss_Robotics_Facility(Races::Protoss, 14);		
	ActionType Protoss_Robotics_Support_Bay(Races::Protoss, 15);		
	ActionType Protoss_Observatory(Races::Protoss, 16);
	ActionType Protoss_Stargate(Races::Protoss, 17);
	ActionType Protoss_Scout(Races::Protoss, 18);
	ActionType Protoss_Arbiter_Tribunal(Races::Protoss, 19);
	ActionType Protoss_Arbiter(Races::Protoss, 20);
	ActionType Protoss_Shield_Battery(Races::Protoss, 21);
	ActionType Protoss_Dark_Templar(Races::Protoss, 22);
	ActionType Protoss_Shuttle(Races::Protoss, 23);
	ActionType Protoss_Reaver(Races::Protoss, 24);
	ActionType Protoss_Observer(Races::Protoss, 25);
	ActionType Protoss_Corsair(Races::Protoss, 26);
	ActionType Protoss_Fleet_Beacon(Races::Protoss, 27);
	ActionType Protoss_Carrier(Races::Protoss, 28);
	ActionType Protoss_Leg_Enhancements(Races::Protoss, 29);
    //ActionType Protoss_Archon(Races::Protoss, 30);

    ActionType Terran_SCV(Races::Terran, 0);
	ActionType Terran_Supply_Depot(Races::Terran, 1);
	ActionType Terran_Command_Center(Races::Terran, 2);
	ActionType Terran_Barracks(Races::Terran, 3);
	ActionType Terran_Refinery(Races::Terran, 4);
	ActionType Terran_Marine(Races::Terran, 5);
	ActionType Terran_Academy(Races::Terran, 6);
	ActionType Terran_Stim_Packs(Races::Terran, 7);
	ActionType Terran_Medic(Races::Terran, 8);
	ActionType Terran_Factory(Races::Terran, 9);
	ActionType Terran_Starport(Races::Terran, 10);
	ActionType Terran_Wraith(Races::Terran, 11);
    ActionType Terran_Siege_Tank(Races::Terran, 12);
    ActionType Terran_Machine_Shop(Races::Terran, 13);
    ActionType Terran_Tank_Siege_Mode(Races::Terran, 14);
    ActionType Terran_Cloaking_Field(Races::Terran, 15);
    ActionType Terran_Control_Tower(Races::Terran, 16);
    ActionType Terran_Battlecruiser(Races::Terran, 17);
    ActionType Terran_Physics_Lab(Races::Terran, 18);
    ActionType Terran_Science_Facility(Races::Terran, 19);
    ActionType Terran_Science_Vessel(Races::Terran, 20);
    ActionType Terran_Vulture(Races::Terran, 21);

    ActionType Zerg_Drone(Races::Zerg, 0);
	ActionType Zerg_Overlord(Races::Zerg, 1);
	ActionType Zerg_Hatchery(Races::Zerg, 2);
	ActionType Zerg_Spawning_Pool(Races::Zerg, 3);
	ActionType Zerg_Zergling(Races::Zerg, 4);
	ActionType Zerg_Extractor(Races::Zerg, 5);
	ActionType Zerg_Lair(Races::Zerg, 6);
	ActionType Zerg_Hydralisk_Den(Races::Zerg, 7);
	ActionType Zerg_Spire(Races::Zerg, 8);
	ActionType Zerg_Hydralisk(Races::Zerg, 9);
	ActionType Zerg_Mutalisk(Races::Zerg, 10);
    ActionType Zerg_Larva(Races::Zerg, 11);
}
}