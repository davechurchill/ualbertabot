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
    ActionType                  requiredAddonType()     const;

    const bool operator == (const ActionType & rhs)     const;
    const bool operator != (const ActionType & rhs)     const;
    const bool operator <  (const ActionType & rhs)     const;
};

class ActionSet;

namespace ActionTypes
{
    void init();
    const ActionSet &  GetAllActionTypes(const RaceID race);
    //const ActionType & GetActionType(const std::string & name);
    const ActionType & GetActionType(const RaceID & race, const ActionID & id);
    const ActionType & GetWorker(const RaceID raceID);
    const ActionType & GetSupplyProvider(const RaceID raceID);
    const ActionType & GetRefinery(const RaceID raceID);
    const ActionType & GetResourceDepot(const RaceID raceID);
    const bool         TypeExists(const BWAPI::UnitType & a);
    const bool         TypeExists(const BWAPI::TechType & a);
    const bool         TypeExists(const BWAPI::UpgradeType & a);

    extern ActionType None;

    extern ActionType Protoss_Probe;
	extern ActionType Protoss_Pylon;
	extern ActionType Protoss_Nexus;
	extern ActionType Protoss_Gateway;
	extern ActionType Protoss_Zealot;
	extern ActionType Protoss_Cybernetics_Core;
	extern ActionType Protoss_Dragoon;
	extern ActionType Protoss_Assimilator;
	extern ActionType Protoss_Singularity_Charge;
	extern ActionType Protoss_Forge;
	extern ActionType Protoss_Photon_Cannon;
	extern ActionType Protoss_High_Templar;
	extern ActionType Protoss_Citadel_of_Adun;
	extern ActionType Protoss_Templar_Archives;
	extern ActionType Protoss_Robotics_Facility;
	extern ActionType Protoss_Robotics_Support_Bay;
	extern ActionType Protoss_Observatory;
	extern ActionType Protoss_Stargate;
	extern ActionType Protoss_Scout;
	extern ActionType Protoss_Arbiter_Tribunal;
	extern ActionType Protoss_Arbiter;
	extern ActionType Protoss_Shield_Battery;
	extern ActionType Protoss_Dark_Templar;
	extern ActionType Protoss_Shuttle;
	extern ActionType Protoss_Reaver;
	extern ActionType Protoss_Observer;
	extern ActionType Protoss_Corsair;
	extern ActionType Protoss_Fleet_Beacon;
	extern ActionType Protoss_Carrier;
	extern ActionType Protoss_Leg_Enhancements;
    extern ActionType Protoss_Archon;

    extern ActionType Terran_SCV;
	extern ActionType Terran_Supply_Depot;
	extern ActionType Terran_Command_Center;
	extern ActionType Terran_Barracks;
	extern ActionType Terran_Refinery;
	extern ActionType Terran_Marine;
	extern ActionType Terran_Academy;
	extern ActionType Terran_Stim_Packs;
	extern ActionType Terran_Medic;
	extern ActionType Terran_Factory;
	extern ActionType Terran_Starport;
	extern ActionType Terran_Wraith;
    extern ActionType Terran_Siege_Tank;
    extern ActionType Terran_Machine_Shop;
    extern ActionType Terran_Tank_Siege_Mode;
    extern ActionType Terran_Cloaking_Field;
    extern ActionType Terran_Control_Tower;
    extern ActionType Terran_Battlecruiser;
    extern ActionType Terran_Physics_Lab;
    extern ActionType Terran_Science_Facility;
    extern ActionType Terran_Science_Vessel;
    extern ActionType Terran_Vulture;

    extern ActionType Zerg_Drone;
	extern ActionType Zerg_Overlord;
	extern ActionType Zerg_Hatchery;
	extern ActionType Zerg_Spawning_Pool;
	extern ActionType Zerg_Zergling;
	extern ActionType Zerg_Extractor;
	extern ActionType Zerg_Lair;
	extern ActionType Zerg_Hydralisk_Den;
	extern ActionType Zerg_Spire;
	extern ActionType Zerg_Hydralisk;
	extern ActionType Zerg_Mutalisk;
}
}