#include "OpeningBuildOrders.h"

using namespace UAlbertaBot;

MetaVector OpeningBuildOrders::ProtossZealotRush()
{
    MetaVector build;
    
    // 0 0 0 0 1 0 3 3 0 0 4 1 4 4 0 4 4 0 1 4 3 0 1 0 4 0 4 4 4 4 1 0 4 4 4
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);
    build.push_back(BWAPI::UnitTypes::Protoss_Zealot);

    return build;
}


MetaVector OpeningBuildOrders::ProtossDarkTemplarRush()
{
    MetaVector build;
    
    // 0 0 0 0 1 0 3 0 7 0 5 0 12 0 13 3 22 22 1 22 22 0 1 0
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Assimilator);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Citadel_of_Adun);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Templar_Archives);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Dark_Templar);
    build.push_back(BWAPI::UnitTypes::Protoss_Dark_Templar);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Dark_Templar);
    build.push_back(BWAPI::UnitTypes::Protoss_Dark_Templar);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);

    return build;
}

MetaVector OpeningBuildOrders::ProtossDragoonRush()
{
    MetaVector build;
    
    // 0 0 0 0 1 0 0 3 0 7 0 0 5 0 0 3 8 6 1 6 6 0 3 1 0 6 6 6
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Assimilator);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UpgradeTypes::Singularity_Charge);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
    build.push_back(BWAPI::UnitTypes::Protoss_Probe);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
    build.push_back(BWAPI::UnitTypes::Protoss_Dragoon);

    return build;
}

MetaVector OpeningBuildOrders::ProtossHighLevelSearch()
{
	MetaVector build;

	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	build.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);
	build.push_back(BWAPI::UnitTypes::Protoss_Probe);


	return build;
}

MetaVector OpeningBuildOrders::TerranMarineRush()
{
    MetaVector build;
    
    // 0 0 0 0 1 0 0 3 0 7 0 0 5 0 0 3 8 6 1 6 6 0 3 1 0 6 6 6
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_Supply_Depot);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_Barracks);
    build.push_back(BWAPI::UnitTypes::Terran_Refinery);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_SCV);
    build.push_back(BWAPI::UnitTypes::Terran_Factory);
    build.push_back(BWAPI::UnitTypes::Terran_Factory);
    build.push_back(BWAPI::UnitTypes::Terran_Machine_Shop);
    build.push_back(BWAPI::UnitTypes::Terran_Machine_Shop);

    return build;
}

MetaVector OpeningBuildOrders::ZergLairRush()
{
    MetaVector build;

    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Overlord);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Hatchery);
    build.push_back(BWAPI::UnitTypes::Zerg_Spawning_Pool);
    build.push_back(BWAPI::UnitTypes::Zerg_Extractor);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Drone);
    build.push_back(BWAPI::UnitTypes::Zerg_Overlord);
    build.push_back(BWAPI::UnitTypes::Zerg_Lair);

    return build;
}